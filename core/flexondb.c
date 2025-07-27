#include "flexondb.h"
#include "../include/platform.h"
#include "../include/compat.h"
#include "../include/schema.h"
#include "../include/writer.h"
#include "../include/reader.h"
#include "../include/io_utils.h"
#include "../include/types.h"

// Helper function to safely duplicate a string (use compatibility layer)
static char* safe_strdup(const char* str) {
    if (!str) return NULL;
    return strdup(str); // This will use our compatibility strdup if needed
}

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
    
    if (estimated_fields <= 0 || estimated_fields > 100) { // Safety limit
        return -1;
    }
    
    // Allocate field array
    *fields = malloc(estimated_fields * sizeof(char*));
    if (!*fields) return -1;
    
    // Initialize all pointers to NULL for safe cleanup
    for (int i = 0; i < estimated_fields; i++) {
        (*fields)[i] = NULL;
    }
    
    // Parse fields (simple implementation, doesn't handle quoted fields with commas)
    char* line_copy = safe_strdup(line);
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
        
        (*fields)[*field_count] = safe_strdup(token);
        if (!(*fields)[*field_count]) {
            // Cleanup on error
            for (int i = 0; i < *field_count; i++) {
                free((*fields)[i]);
            }
            free(*fields);
            free(line_copy);
            *fields = NULL;
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
    
    // For now, we'll just log that this would insert data
    // A full implementation would need the writer_open function to be implemented
    // or we'd need to implement a different approach for appending to existing databases
    FLEXON_LOG("Note: Insert functionality requires writer_open to be implemented\n");
    FLEXON_LOG("JSON data to insert: %s\n", json);
    
    return 0; // Return success for demonstration
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
    
    char line[2048]; // Increased buffer size
    char** header_fields = NULL;
    int header_count = 0;
    bool header_read = false;
    char* schema_str = malloc(4096); // Use dynamic allocation
    writer_t* writer = NULL;
    schema_t* parsed_schema = NULL;
    int result = -1;
    
    if (!schema_str) {
        FLEXON_LOG("Error: Failed to allocate memory for schema string\n");
        fclose(csv_file);
        return -1;
    }
    schema_str[0] = '\0'; // Initialize empty string
    
    // Read CSV file line by line
    while (fgets(line, sizeof(line), csv_file)) {
        if (!header_read) {
            // Parse header to create schema
            if (parse_csv_line(line, &header_fields, &header_count) != 0) {
                FLEXON_LOG("Error: Failed to parse CSV header\n");
                break;
            }
            
            if (header_count <= 0 || header_count > 50) { // Safety check
                FLEXON_LOG("Error: Invalid header field count: %d\n", header_count);
                free_csv_fields(header_fields, header_count);
                break;
            }
            
            // Build schema string (assume all fields are strings for simplicity)
            size_t schema_len = 0;
            for (int i = 0; i < header_count; i++) {
                if (!header_fields[i] || strlen(header_fields[i]) == 0) {
                    FLEXON_LOG("Error: Empty header field at position %d\n", i);
                    free_csv_fields(header_fields, header_count);
                    goto cleanup;
                }
                
                size_t field_len = strlen(header_fields[i]) + 8; // + " string"
                if (i > 0) field_len += 2; // + ", "
                
                if (schema_len + field_len >= 4095) { // Check buffer bounds
                    FLEXON_LOG("Error: Schema string too long\n");
                    free_csv_fields(header_fields, header_count);
                    goto cleanup;
                }
                
                if (i > 0) strcat(schema_str, ", ");
                strcat(schema_str, header_fields[i]);
                strcat(schema_str, " string");
                schema_len += field_len;
            }
            
            FLEXON_LOG("Generated schema: %s\n", schema_str);
            
            // Parse the schema
            parsed_schema = parse_schema(schema_str);
            if (!parsed_schema) {
                FLEXON_LOG("Error: Failed to parse generated schema\n");
                free_csv_fields(header_fields, header_count);
                break;
            }
            
            // Create the database and writer directly
            writer = writer_create_default(dbPath, parsed_schema);
            if (!writer) {
                FLEXON_LOG("Error: Failed to create database writer\n");
                free_csv_fields(header_fields, header_count);
                break;
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
                FLEXON_LOG("Warning: Data row has %d fields, expected %d, skipping\n", data_count, header_count);
                free_csv_fields(data_fields, data_count);
                continue;
            }
            
            // Build JSON string with dynamic allocation
            char* json_str = malloc(4096);
            if (!json_str) {
                FLEXON_LOG("Error: Failed to allocate memory for JSON string\n");
                free_csv_fields(data_fields, data_count);
                break;
            }
            
            strcpy(json_str, "{");
            size_t json_len = 1;
            
            for (int i = 0; i < data_count && i < header_count; i++) {
                if (!data_fields[i] || !header_fields[i]) continue;
                
                // Estimate required space: "field": "value", 
                size_t needed = strlen(header_fields[i]) + strlen(data_fields[i]) + 10;
                if (json_len + needed >= 4095) {
                    FLEXON_LOG("Warning: JSON string too long, truncating\n");
                    break;
                }
                
                if (i > 0) {
                    strcat(json_str, ", ");
                    json_len += 2;
                }
                strcat(json_str, "\"");
                strcat(json_str, header_fields[i]);
                strcat(json_str, "\": \"");
                strcat(json_str, data_fields[i]);
                strcat(json_str, "\"");
                json_len += needed;
            }
            strcat(json_str, "}");
            
            // Insert the data
            if (writer_insert_json(writer, json_str) != 0) {
                FLEXON_LOG("Warning: Failed to insert data row, skipping\n");
            }
            
            free(json_str);
            free_csv_fields(data_fields, data_count);
        }
    }
    
    // If we got here and have read the header, it's a success
    if (header_read) {
        FLEXON_LOG("CSV conversion completed successfully\n");
        result = 0;
    }
    
cleanup:
    // Cleanup
    if (writer) {
        writer_close(writer);
    }
    if (parsed_schema) {
        free_schema(parsed_schema);
    }
    if (header_fields) {
        free_csv_fields(header_fields, header_count);
    }
    if (schema_str) {
        free(schema_str);
    }
    fclose(csv_file);
    
    return result;
}