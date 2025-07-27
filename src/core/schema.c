#define _GNU_SOURCE
#include "../../include/schema.h"
#include "../../include/writer.h"
#include "../../include/utils.h"
#include "../../include/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// Helper function to trim whitespace
static char* trim_whitespace(char* str) {
    char* end;
    
    // Trim leading space
    while(isspace((unsigned char)*str)) str++;
    
    if(*str == 0) return str;
    
    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    
    end[1] = '\0';
    return str;
}

// Parse field type from string
field_type_t string_to_field_type(const char* type_str) {
    if (strcmp(type_str, "int32") == 0) {
        return FIELD_TYPE_INT32;
    } else if (strcmp(type_str, "float") == 0) {
        return FIELD_TYPE_FLOAT;
    } else if (strcmp(type_str, "string") == 0) {
        return FIELD_TYPE_STRING;
    } else if (strcmp(type_str, "bool") == 0) {
        return FIELD_TYPE_BOOL;
    }
    return FIELD_TYPE_UNKNOWN;
}

// Get string representation of field type
const char* field_type_to_string(field_type_t type) {
    switch(type) {
        case FIELD_TYPE_INT32:  return "int32";
        case FIELD_TYPE_FLOAT:  return "float";
        case FIELD_TYPE_STRING: return "string";
        case FIELD_TYPE_BOOL:   return "bool";
        default:                return "unknown";
    }
}

// Get field size in bytes
static uint32_t get_field_size(field_type_t type) {
    switch(type) {
        case FIELD_TYPE_INT32:  return 4;
        case FIELD_TYPE_FLOAT:  return 4;
        case FIELD_TYPE_STRING: return 256; // Default string size, can be configurable
        case FIELD_TYPE_BOOL:   return 1;
        default:                return 0;
    }
}

// Parse a schema string like "name string, age int32, salary float"
schema_t* parse_schema(const char* schema_str) {
    if (!schema_str || strlen(schema_str) == 0) {
        fprintf(stderr, "Error: Empty schema string\n");
        return NULL;
    }
    
    schema_t* schema = calloc(1, sizeof(schema_t));
    if (!schema) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    
    // Initialize schema
    schema->field_count = 0;
    schema->row_size = 0;
    schema->raw_schema_str = strdup(schema_str);
    
    // Create a copy of schema string for parsing
    char* schema_copy = strdup(schema_str);
    if (!schema_copy) {
        free_schema(schema);
        return NULL;
    }
    
    // Parse fields separated by commas
    char* token = strtok(schema_copy, ",");
    while (token && schema->field_count < MAX_SCHEMA_FIELDS) {
        token = trim_whitespace(token);
        
        // Split field definition by space (name type)
        char* space_pos = strrchr(token, ' ');
        if (!space_pos) {
            fprintf(stderr, "Error: Invalid field definition '%s'\n", token);
            free(schema_copy);
            free_schema(schema);
            return NULL;
        }
        
        *space_pos = '\0';
        char* field_name = trim_whitespace(token);
        char* type_str = trim_whitespace(space_pos + 1);
        
        // Validate field name length
        if (strlen(field_name) >= MAX_FIELD_NAME_LEN) {
            fprintf(stderr, "Error: Field name '%s' too long\n", field_name);
            free(schema_copy);
            free_schema(schema);
            return NULL;
        }
        
        // Parse field type
        field_type_t type = string_to_field_type(type_str);
        if (type == FIELD_TYPE_UNKNOWN) {
            fprintf(stderr, "Error: Unknown field type '%s'\n", type_str);
            free(schema_copy);
            free_schema(schema);
            return NULL;
        }
        
        // Add field to schema
        field_def_t* field = &schema->fields[schema->field_count];
        strcpy(field->name, field_name);
        field->type = type;
        field->size = get_field_size(type);
        
        schema->field_count++;
        token = strtok(NULL, ",");
    }
    
    free(schema_copy);
    
    if (schema->field_count == 0) {
        fprintf(stderr, "Error: No valid fields found in schema\n");
        free_schema(schema);
        return NULL;
    }
    
    // Calculate total row size
    schema->row_size = calculate_row_size(schema);
    
    // Validate schema
    if (!validate_schema(schema)) {
        free_schema(schema);
        return NULL;
    }
    
    return schema;
}

// Calculate row size based on schema fields
uint32_t calculate_row_size(const schema_t* schema) {
    if (!schema) return 0;
    
    uint32_t total_size = 0;
    for (uint32_t i = 0; i < schema->field_count; i++) {
        total_size += schema->fields[i].size;
    }
    return total_size;
}

// Validate schema
bool validate_schema(const schema_t* schema) {
    if (!schema || schema->field_count == 0) {
        return false;
    }
    
    // Check for duplicate field names
    for (uint32_t i = 0; i < schema->field_count; i++) {
        for (uint32_t j = i + 1; j < schema->field_count; j++) {
            if (strcmp(schema->fields[i].name, schema->fields[j].name) == 0) {
                fprintf(stderr, "Error: Duplicate field name '%s'\n", schema->fields[i].name);
                return false;
            }
        }
    }
    
    return true;
}

// Get field index by name
int get_field_index(const schema_t* schema, const char* field_name) {
    if (!schema || !field_name) return -1;
    
    for (uint32_t i = 0; i < schema->field_count; i++) {
        if (strcmp(schema->fields[i].name, field_name) == 0) {
            return (int)i;
        }
    }
    return -1;
}

// Print schema information
void print_schema(const schema_t* schema) {
    if (!schema) {
        printf("Schema: NULL\n");
        return;
    }
    
    printf("Schema (%u fields, %u bytes per row):\n", schema->field_count, schema->row_size);
    printf("┌─────────────────────────────────┬──────────┬───────────┐\n");
    printf("│ Field Name                      │ Type     │ Size (B)  │\n");
    printf("├─────────────────────────────────┼──────────┼───────────┤\n");
    
    for (uint32_t i = 0; i < schema->field_count; i++) {
        const field_def_t* field = &schema->fields[i];
        printf("│ %-31s │ %-8s │ %-9u │\n", 
               field->name, 
               field_type_to_string(field->type), 
               field->size);
    }
    
    printf("└─────────────────────────────────┴──────────┴───────────┘\n");
    
    if (schema->raw_schema_str) {
        printf("Raw schema: %s\n", schema->raw_schema_str);
    }
}

// Free memory allocated for schema
void free_schema(schema_t* schema) {
    if (schema) {
        if (schema->raw_schema_str) {
            free(schema->raw_schema_str);
        }
        free(schema);
    }
}

// Load schema from .fxdb file
schema_t* load_schema(const char* filename) {
    if (!filename) {
        return NULL;
    }
    
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s': %s\n", filename, strerror(errno));
        return NULL;
    }
    
    // Read header
    fxdb_header_t header;
    if (fread(&header, sizeof(fxdb_header_t), 1, file) != 1) {
        fprintf(stderr, "Error: Cannot read header from '%s'\n", filename);
        fclose(file);
        return NULL;
    }
    
    // Validate magic number
    if (header.magic != FXDB_MAGIC_NUM) {
        fprintf(stderr, "Error: Invalid file format - not a FlexonDB file\n");
        fclose(file);
        return NULL;
    }
    
    // Seek to schema section
    if (fseek(file, header.schema_offset, SEEK_SET) != 0) {
        fprintf(stderr, "Error: Cannot seek to schema in '%s'\n", filename);
        fclose(file);
        return NULL;
    }
    
    // Read schema metadata
    uint32_t field_count, row_size, schema_str_len;
    if (fread(&field_count, sizeof(uint32_t), 1, file) != 1 ||
        fread(&row_size, sizeof(uint32_t), 1, file) != 1 ||
        fread(&schema_str_len, sizeof(uint32_t), 1, file) != 1) {
        fprintf(stderr, "Error: Cannot read schema metadata from '%s'\n", filename);
        fclose(file);
        return NULL;
    }
    
    // Read schema string
    char* schema_str = malloc(schema_str_len + 1);
    if (!schema_str) {
        fclose(file);
        return NULL;
    }
    
    if (fread(schema_str, 1, schema_str_len, file) != schema_str_len) {
        fprintf(stderr, "Error: Cannot read schema string from '%s'\n", filename);
        free(schema_str);
        fclose(file);
        return NULL;
    }
    schema_str[schema_str_len] = '\0';
    
    fclose(file);
    
    // Parse the schema string
    schema_t* schema = parse_schema(schema_str);
    free(schema_str);
    
    if (!schema) {
        fprintf(stderr, "Error: Cannot parse schema from '%s'\n", filename);
        return NULL;
    }
    
    // Validate parsed schema matches file metadata
    if (schema->field_count != field_count || schema->row_size != row_size) {
        fprintf(stderr, "Error: Schema validation failed for '%s'\n", filename);
        free_schema(schema);
        return NULL;
    }
    
    return schema;
}

// Save schema to .fxdb file
int save_schema(const char* filename, const schema_t* schema) {
    if (!filename || !schema) {
        return -1;
    }
    
    FILE* file = fopen(filename, "r+b");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s' for writing: %s\n", filename, strerror(errno));
        return -1;
    }
    
    // Read current header to get schema offset
    fxdb_header_t header;
    if (fread(&header, sizeof(fxdb_header_t), 1, file) != 1) {
        fprintf(stderr, "Error: Cannot read header from '%s'\n", filename);
        fclose(file);
        return -1;
    }
    
    // Validate magic number
    if (header.magic != FXDB_MAGIC_NUM) {
        fprintf(stderr, "Error: Invalid file format - not a FlexonDB file\n");
        fclose(file);
        return -1;
    }
    
    // Seek to schema section
    if (fseek(file, header.schema_offset, SEEK_SET) != 0) {
        fprintf(stderr, "Error: Cannot seek to schema in '%s'\n", filename);
        fclose(file);
        return -1;
    }
    
    // Write schema metadata
    uint32_t field_count = schema->field_count;
    uint32_t row_size = schema->row_size;
    uint32_t schema_str_len = schema->raw_schema_str ? strlen(schema->raw_schema_str) : 0;
    
    if (fwrite(&field_count, sizeof(uint32_t), 1, file) != 1 ||
        fwrite(&row_size, sizeof(uint32_t), 1, file) != 1 ||
        fwrite(&schema_str_len, sizeof(uint32_t), 1, file) != 1) {
        fprintf(stderr, "Error: Cannot write schema metadata to '%s'\n", filename);
        fclose(file);
        return -1;
    }
    
    // Write schema string if available
    if (schema->raw_schema_str && schema_str_len > 0) {
        if (fwrite(schema->raw_schema_str, 1, schema_str_len, file) != schema_str_len) {
            fprintf(stderr, "Error: Cannot write schema string to '%s'\n", filename);
            fclose(file);
            return -1;
        }
    }
    
    // Write field definitions
    for (uint32_t i = 0; i < schema->field_count; i++) {
        const field_def_t* field = &schema->fields[i];
        
        if (fwrite(field->name, 1, MAX_FIELD_NAME_LEN, file) != MAX_FIELD_NAME_LEN ||
            fwrite(&field->type, sizeof(field_type_t), 1, file) != 1 ||
            fwrite(&field->size, sizeof(uint32_t), 1, file) != 1) {
            fprintf(stderr, "Error: Cannot write field definition %u to '%s'\n", i, filename);
            fclose(file);
            return -1;
        }
    }
    
    fclose(file);
    return 0;
}