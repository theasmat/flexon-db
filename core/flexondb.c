#include "flexondb.h"

// Cross-platform includes
#ifdef __ANDROID__
    #include <android/log.h>
    #define FLEXON_LOG(...) __android_log_print(ANDROID_LOG_INFO, "FlexonDB", __VA_ARGS__)
#elif __APPLE__
    #include <os/log.h>
    #define FLEXON_LOG(...) os_log(OS_LOG_DEFAULT, __VA_ARGS__)
#else
    #define FLEXON_LOG(...) printf(__VA_ARGS__)
#endif

// Enable GNU extensions for strdup
#define _GNU_SOURCE

#include "../include/schema.h"
#include "../include/writer.h"
#include "../include/reader.h"
#include "../include/io_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Helper function to build a simple CSV parser
static int parse_csv_line(const char* line, char*** fields, int* field_count) {
    if (!line || !fields || !field_count) return -1;
    
    *field_count = 0;
    *fields = NULL;
    
    // Count commas to estimate field count
    int comma_count = 0;
    for (const char* p = line; *p; p++) {
        if (*p == ',') comma_count++;
    }
    int estimated_fields = comma_count + 1;
    
    // Allocate field array
    *fields = malloc(estimated_fields * sizeof(char*));
    if (!*fields) return -1;
    
    // Parse fields (simple implementation, doesn't handle quoted fields with commas)
    char* line_copy = strdup(line);
    if (!line_copy) {
        free(*fields);
        *fields = NULL;
        return -1;
    }
    
    char* token = strtok(line_copy, ",");
    while (token && *field_count < estimated_fields) {
        // Trim whitespace
        while (*token == ' ' || *token == '\t') token++;
        char* end = token + strlen(token) - 1;
        while (end > token && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
            *end = '\0';
            end--;
        }
        
        (*fields)[*field_count] = strdup(token);
        if (!(*fields)[*field_count]) {
            // Cleanup on error
            for (int i = 0; i < *field_count; i++) {
                free((*fields)[i]);
            }
            free(*fields);
            free(line_copy);
            return -1;
        }
        (*field_count)++;
        token = strtok(NULL, ",");
    }
    
    free(line_copy);
    return 0;
}

// Helper function to free parsed CSV fields
static void free_csv_fields(char** fields, int field_count) {
    if (!fields) return;
    for (int i = 0; i < field_count; i++) {
        free(fields[i]);
    }
    free(fields);
}

int createDatabase(const char* path, const char* schema) {
    if (!path || !schema) {
        FLEXON_LOG("Error: Invalid path or schema\n");
        return -1;
    }
    
    FLEXON_LOG("Creating database at %s with schema: %s\n", path, schema);
    
    // Parse the schema string
    schema_t* parsed_schema = parse_schema(schema);
    if (!parsed_schema) {
        FLEXON_LOG("Error: Failed to parse schema\n");
        return -1;
    }
    
    // Create the database using the enhanced creation function
    int result = fxdb_database_create(path, parsed_schema, NULL);
    
    free_schema(parsed_schema);
    
    if (result != 0) {
        FLEXON_LOG("Error: Failed to create database\n");
        return -1;
    }
    
    return 0;
}

int insertData(const char* path, const char* json) {
    if (!path || !json) {
        FLEXON_LOG("Error: Invalid path or JSON data\n");
        return -1;
    }
    
    FLEXON_LOG("Inserting data into %s: %s\n", path, json);
    
    // Open the database for writing
    writer_t* writer = writer_open(path);
    if (!writer) {
        FLEXON_LOG("Error: Failed to open database for writing\n");
        return -1;
    }
    
    // Insert the JSON data
    int result = writer_insert_json(writer, json);
    if (result != 0) {
        FLEXON_LOG("Error: Failed to insert JSON data\n");
        writer_close(writer);
        return -1;
    }
    
    // Close the writer
    result = writer_close(writer);
    if (result != 0) {
        FLEXON_LOG("Error: Failed to close database writer\n");
        return -1;
    }
    
    return 0;
}

char* readData(const char* path) {
    if (!path) {
        FLEXON_LOG("Error: Invalid path\n");
        return NULL;
    }
    
    // Open the database for reading
    reader_t* reader = reader_open(path);
    if (!reader) {
        FLEXON_LOG("Error: Failed to open database for reading\n");
        return NULL;
    }
    
    // Get total row count
    uint32_t total_rows = reader_get_row_count(reader);
    
    // Create a result string (simplified implementation)
    char* result = malloc(1024);
    if (!result) {
        reader_close(reader);
        return NULL;
    }
    
    snprintf(result, 1024, "Data read from %s - Contains %u rows", path, total_rows);
    
    reader_close(reader);
    return result;
}

int deleteDatabase(const char* path) {
    if (!path) {
        FLEXON_LOG("Error: Invalid path\n");
        return -1;
    }
    
    FLEXON_LOG("Deleting database at %s\n", path);
    
    // Use the enhanced database deletion function
    int result = fxdb_database_delete(path);
    if (result != 0) {
        FLEXON_LOG("Error: Failed to delete database\n");
        return -1;
    }
    
    return 0;
}

int updateDatabase(const char* path, const char* json) {
    if (!path || !json) {
        FLEXON_LOG("Error: Invalid path or JSON data\n");
        return -1;
    }
    
    FLEXON_LOG("Updating database at %s with new data: %s\n", path, json);
    
    // For now, implement update as insert (append new data)
    // In a full implementation, this would update existing records
    int result = insertData(path, json);
    if (result != 0) {
        FLEXON_LOG("Error: Failed to update database\n");
        return -1;
    }
    
    return 0;
}

int csvToFlexonDB(const char* csvPath, const char* dbPath) {
    if (!csvPath || !dbPath) {
        FLEXON_LOG("Error: Invalid CSV path or database path\n");
        return -1;
    }
    
    FLEXON_LOG("Converting CSV file %s to FlexonDB at %s\n", csvPath, dbPath);
    
    FILE* csv_file = fopen(csvPath, "r");
    if (!csv_file) {
        FLEXON_LOG("Error: Failed to open CSV file %s\n", csvPath);
        return -1;
    }
    
    char line[1024];
    char** header_fields = NULL;
    int header_count = 0;
    bool header_read = false;
    char schema_str[2048] = "";
    writer_t* writer = NULL;
    
    // Read CSV file line by line
    while (fgets(line, sizeof(line), csv_file)) {
        if (!header_read) {
            // Parse header to create schema
            if (parse_csv_line(line, &header_fields, &header_count) != 0) {
                FLEXON_LOG("Error: Failed to parse CSV header\n");
                fclose(csv_file);
                return -1;
            }
            
            // Build schema string (assume all fields are strings for simplicity)
            for (int i = 0; i < header_count; i++) {
                if (i > 0) strcat(schema_str, ", ");
                strcat(schema_str, header_fields[i]);
                strcat(schema_str, " string");
            }
            
            // Create the database
            if (createDatabase(dbPath, schema_str) != 0) {
                FLEXON_LOG("Error: Failed to create database from CSV schema\n");
                free_csv_fields(header_fields, header_count);
                fclose(csv_file);
                return -1;
            }
            
            // Open database for writing
            writer = writer_open(dbPath);
            if (!writer) {
                FLEXON_LOG("Error: Failed to open database for writing\n");
                free_csv_fields(header_fields, header_count);
                fclose(csv_file);
                return -1;
            }
            
            header_read = true;
        } else {
            // Parse data row and insert as JSON
            char** data_fields = NULL;
            int data_count = 0;
            
            if (parse_csv_line(line, &data_fields, &data_count) != 0) {
                FLEXON_LOG("Warning: Failed to parse CSV data line, skipping\n");
                continue;
            }
            
            if (data_count != header_count) {
                FLEXON_LOG("Warning: Data row has different field count than header, skipping\n");
                free_csv_fields(data_fields, data_count);
                continue;
            }
            
            // Build JSON string
            char json_str[2048] = "{";
            for (int i = 0; i < data_count && i < header_count; i++) {
                if (i > 0) strcat(json_str, ", ");
                strcat(json_str, "\"");
                strcat(json_str, header_fields[i]);
                strcat(json_str, "\": \"");
                strcat(json_str, data_fields[i]);
                strcat(json_str, "\"");
            }
            strcat(json_str, "}");
            
            // Insert the data
            if (writer_insert_json(writer, json_str) != 0) {
                FLEXON_LOG("Warning: Failed to insert data row, skipping\n");
            }
            
            free_csv_fields(data_fields, data_count);
        }
    }
    
    // Cleanup
    if (writer) {
        writer_close(writer);
    }
    if (header_fields) {
        free_csv_fields(header_fields, header_count);
    }
    fclose(csv_file);
    
    FLEXON_LOG("CSV conversion completed successfully\n");
    return 0;
}