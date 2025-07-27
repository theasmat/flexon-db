#include "../../include/writer.h"
#include "../../include/io_utils.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

// Create default writer configuration
writer_config_t writer_default_config(void) {
    writer_config_t config = {
        .chunk_size = DEFAULT_CHUNK_SIZE,
        .use_compression = false,
        .build_index = false
    };
    return config;
}

// Write file header to disk
static int write_header(writer_t* writer) {
    if (fseek(writer->file, 0, SEEK_SET) != 0) {
        return -1;
    }
    
    size_t written = fwrite(&writer->header, sizeof(fxdb_header_t), 1, writer->file);
    if (written != 1) {
        return -1;
    }
    
    return 0;
}

// Write schema to file
static int write_schema(writer_t* writer) {
    if (!writer->schema || !writer->schema->raw_schema_str) {
        return -1;
    }
    
    // Move to schema position
    if (fseek(writer->file, writer->header.schema_offset, SEEK_SET) != 0) {
        return -1;
    }
    
    // Write schema metadata
    uint32_t field_count = writer->schema->field_count;
    uint32_t row_size = writer->schema->row_size;
    uint32_t schema_str_len = strlen(writer->schema->raw_schema_str);
    
    if (fwrite(&field_count, sizeof(uint32_t), 1, writer->file) != 1 ||
        fwrite(&row_size, sizeof(uint32_t), 1, writer->file) != 1 ||
        fwrite(&schema_str_len, sizeof(uint32_t), 1, writer->file) != 1) {
        return -1;
    }
    
    // Write schema string
    if (fwrite(writer->schema->raw_schema_str, 1, schema_str_len, writer->file) != schema_str_len) {
        return -1;
    }
    
    // Write field definitions
    for (uint32_t i = 0; i < writer->schema->field_count; i++) {
        const field_def_t* field = &writer->schema->fields[i];
        
        if (fwrite(field->name, 1, MAX_FIELD_NAME_LEN, writer->file) != MAX_FIELD_NAME_LEN ||
            fwrite(&field->type, sizeof(field_type_t), 1, writer->file) != 1 ||
            fwrite(&field->size, sizeof(uint32_t), 1, writer->file) != 1) {
            return -1;
        }
    }
    
    return 0;
}

// Create a new .fxdb file with schema
writer_t* writer_create(const char* filename, const schema_t* schema, const writer_config_t* config) {
    if (!filename || !schema) {
        return NULL;
    }
    
    writer_t* writer = malloc(sizeof(writer_t));
    if (!writer) {
        return NULL;
    }
    
    // Initialize writer
    memset(writer, 0, sizeof(writer_t));
    writer->schema = (schema_t*)schema; // Note: We don't own the schema
    writer->config = config ? *config : writer_default_config();
    
    // Open file for writing
    writer->file = fopen(filename, "wb");
    if (!writer->file) {
        fprintf(stderr, "Error: Cannot create file '%s': %s\n", filename, strerror(errno));
        free(writer);
        return NULL;
    }
    
    // Initialize header
    writer->header.magic = FXDB_MAGIC_NUM;
    writer->header.version = FXDB_VERSION;
    writer->header.chunk_size = writer->config.chunk_size;
    writer->header.total_rows = 0;
    writer->header.chunk_count = 0;
    
    // Calculate offsets
    writer->header.schema_offset = sizeof(fxdb_header_t);
    writer->header.schema_size = 3 * sizeof(uint32_t) + // metadata
                                strlen(schema->raw_schema_str) +
                                schema->field_count * (MAX_FIELD_NAME_LEN + sizeof(field_type_t) + sizeof(uint32_t));
    
    writer->header.data_offset = writer->header.schema_offset + writer->header.schema_size;
    writer->header.data_size = 0; // Will be updated as we write
    writer->header.index_offset = 0; // No index initially
    writer->header.index_size = 0;
    
    // Allocate row buffer
    size_t buffer_size = writer->config.chunk_size * schema->row_size;
    writer->row_buffer = malloc(buffer_size);
    if (!writer->row_buffer) {
        fclose(writer->file);
        free(writer);
        return NULL;
    }
    
    // Write initial header (will be updated later)
    if (write_header(writer) != 0) {
        free(writer->row_buffer);
        fclose(writer->file);
        free(writer);
        return NULL;
    }
    
    // Write schema
    if (write_schema(writer) != 0) {
        free(writer->row_buffer);
        fclose(writer->file);
        free(writer);
        return NULL;
    }
    
    // Position at data section
    writer->data_start_pos = writer->header.data_offset;
    if (fseek(writer->file, writer->data_start_pos, SEEK_SET) != 0) {
        free(writer->row_buffer);
        fclose(writer->file);
        free(writer);
        return NULL;
    }
    
    return writer;
}

// Create writer with default configuration
writer_t* writer_create_default(const char* filename, const schema_t* schema) {
    writer_config_t config = writer_default_config();
    return writer_create(filename, schema, &config);
}

// Serialize row data into buffer
int serialize_row(const schema_t* schema, const field_value_t* values, uint32_t value_count, uint8_t* buffer) {
    if (!schema || !values || !buffer) {
        return -1;
    }
    
    uint32_t offset = 0;
    
    for (uint32_t i = 0; i < schema->field_count; i++) {
        const field_def_t* field = &schema->fields[i];
        const field_value_t* value = NULL;
        
        // Find value for this field
        for (uint32_t j = 0; j < value_count; j++) {
            if (strcmp(values[j].field_name, field->name) == 0) {
                value = &values[j];
                break;
            }
        }
        
        if (!value) {
            fprintf(stderr, "Error: Missing value for field '%s'\n", field->name);
            return -1;
        }
        
        // Serialize based on type
        switch (field->type) {
            case FIELD_TYPE_INT32:
                memcpy(buffer + offset, &value->value.int32_val, sizeof(int32_t));
                offset += sizeof(int32_t);
                break;
                
            case FIELD_TYPE_FLOAT:
                memcpy(buffer + offset, &value->value.float_val, sizeof(float));
                offset += sizeof(float);
                break;
                
            case FIELD_TYPE_BOOL:
                buffer[offset] = value->value.bool_val ? 1 : 0;
                offset += 1;
                break;
                
            case FIELD_TYPE_STRING:
                // Copy string with null padding
                memset(buffer + offset, 0, field->size);
                if (value->value.string_val) {
                    size_t len = strlen(value->value.string_val);
                    if (len >= field->size) {
                        len = field->size - 1; // Leave room for null terminator
                    }
                    memcpy(buffer + offset, value->value.string_val, len);
                }
                offset += field->size;
                break;
                
            default:
                fprintf(stderr, "Error: Unknown field type %d\n", field->type);
                return -1;
        }
    }
    
    return offset;
}

// Insert a row using field values array
int writer_insert_row(writer_t* writer, const field_value_t* values, uint32_t value_count) {
    if (!writer || !values) {
        return -1;
    }
    
    // Serialize row into buffer
    uint8_t* row_pos = writer->row_buffer + (writer->buffer_row_count * writer->schema->row_size);
    int bytes_written = serialize_row(writer->schema, values, value_count, row_pos);
    
    if (bytes_written < 0) {
        return -1;
    }
    
    writer->buffer_row_count++;
    writer->total_rows++;
    
    // Flush chunk if buffer is full
    if (writer->buffer_row_count >= writer->config.chunk_size) {
        return writer_flush_chunk(writer);
    }
    
    return 0;
}

// Flush current chunk to disk
int writer_flush_chunk(writer_t* writer) {
    if (!writer || writer->buffer_row_count == 0) {
        return 0; // Nothing to flush
    }
    
    // Write chunk header
    uint32_t chunk_header[2] = {
        writer->buffer_row_count,                           // rows in chunk
        writer->buffer_row_count * writer->schema->row_size // chunk size in bytes
    };
    
    if (fwrite(chunk_header, sizeof(uint32_t), 2, writer->file) != 2) {
        return -1;
    }
    
    // Write chunk data
    size_t chunk_data_size = writer->buffer_row_count * writer->schema->row_size;
    if (fwrite(writer->row_buffer, 1, chunk_data_size, writer->file) != chunk_data_size) {
        return -1;
    }
    
    // Update statistics
    writer->header.chunk_count++;
    writer->header.data_size += sizeof(chunk_header) + chunk_data_size;
    
    // Reset buffer
    writer->buffer_row_count = 0;
    writer->current_chunk++;
    
    return 0;
}

// Get writer statistics
void writer_get_stats(const writer_t* writer, uint32_t* total_rows, uint32_t* chunks_written) {
    if (writer) {
        if (total_rows) *total_rows = writer->total_rows;
        if (chunks_written) *chunks_written = writer->header.chunk_count;
    }
}

// Close writer and finalize file
int writer_close(writer_t* writer) {
    if (!writer) {
        return -1;
    }
    
    // Flush any remaining data
    if (writer->buffer_row_count > 0) {
        if (writer_flush_chunk(writer) != 0) {
            return -1;
        }
    }
    
    // Update header with final statistics
    writer->header.total_rows = writer->total_rows;
    
    // Write final header
    if (write_header(writer) != 0) {
        return -1;
    }
    
    // Close file
    if (fclose(writer->file) != 0) {
        return -1;
    }
    
    writer->file = NULL;
    return 0;
}

// Free writer resources
void writer_free(writer_t* writer) {
    if (writer) {
        if (writer->file) {
            fclose(writer->file);
        }
        if (writer->row_buffer) {
            free(writer->row_buffer);
        }
        free(writer);
    }
}

// Helper function to trim whitespace
static char* trim_whitespace(char* str) {
    char* end;
    
    // Trim leading space
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') {
        str++;
    }
    
    if (*str == 0) return str;
    
    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        end--;
    }
    end[1] = '\0';
    
    return str;
}

// Helper function to parse JSON value and convert to appropriate type
static int parse_json_value(const char* value_str, field_type_t type, field_value_t* out_value) {
    char* trimmed = trim_whitespace((char*)value_str);
    
    switch (type) {
        case TYPE_STRING: {
            // Remove quotes if present
            if (trimmed[0] == '"' && trimmed[strlen(trimmed) - 1] == '"') {
                trimmed[strlen(trimmed) - 1] = '\0';
                trimmed++;
            }
            out_value->value.string_val = trimmed;
            break;
        }
        case TYPE_INT32: {
            char* endptr;
            long val = strtol(trimmed, &endptr, 10);
            if (*endptr != '\0' || val > INT32_MAX || val < INT32_MIN) {
                return -1;
            }
            out_value->value.int32_val = (int32_t)val;
            break;
        }
        case TYPE_FLOAT: {
            char* endptr;
            float val = strtof(trimmed, &endptr);
            if (*endptr != '\0') {
                return -1;
            }
            out_value->value.float_val = val;
            break;
        }
        case TYPE_BOOL: {
            if (strcmp(trimmed, "true") == 0 || strcmp(trimmed, "1") == 0) {
                out_value->value.bool_val = true;
            } else if (strcmp(trimmed, "false") == 0 || strcmp(trimmed, "0") == 0) {
                out_value->value.bool_val = false;
            } else {
                return -1;
            }
            break;
        }
        default:
            return -1;
    }
    
    return 0;
}

// Simple JSON parser for insert (basic implementation)
int writer_insert_json(writer_t* writer, const char* json_str) {
    if (!writer || !json_str) {
        return -1;
    }
    
    // Create a working copy of the JSON string
    char* json_copy = malloc(strlen(json_str) + 1);
    if (!json_copy) {
        return -1;
    }
    strcpy(json_copy, json_str);
    
    // Trim and validate basic JSON structure
    char* trimmed = trim_whitespace(json_copy);
    if (trimmed[0] != '{' || trimmed[strlen(trimmed) - 1] != '}') {
        fprintf(stderr, "Error: Invalid JSON format - must be an object {...}\n");
        free(json_copy);
        return -1;
    }
    
    // Remove outer braces
    trimmed[strlen(trimmed) - 1] = '\0';
    trimmed++;
    
    // Prepare field values array
    field_value_t* values = malloc(writer->schema->field_count * sizeof(field_value_t));
    if (!values) {
        free(json_copy);
        return -1;
    }
    
    // Initialize all fields to ensure we have values for all schema fields
    for (uint32_t i = 0; i < writer->schema->field_count; i++) {
        values[i].field_name = writer->schema->fields[i].name;
        // Set default values based on type
        switch (writer->schema->fields[i].type) {
            case TYPE_STRING:
                values[i].value.string_val = "";
                break;
            case TYPE_INT32:
                values[i].value.int32_val = 0;
                break;
            case TYPE_FLOAT:
                values[i].value.float_val = 0.0f;
                break;
            case TYPE_BOOL:
                values[i].value.bool_val = false;
                break;
            default:
                break;
        }
    }
    
    // Parse key-value pairs
    char* pair = strtok(trimmed, ",");
    while (pair != NULL) {
        // Find the key-value separator
        char* colon = strchr(pair, ':');
        if (!colon) {
            fprintf(stderr, "Error: Invalid JSON pair format\n");
            free(values);
            free(json_copy);
            return -1;
        }
        
        *colon = '\0';
        char* key = trim_whitespace(pair);
        char* value = trim_whitespace(colon + 1);
        
        // Remove quotes from key if present
        if (key[0] == '"' && key[strlen(key) - 1] == '"') {
            key[strlen(key) - 1] = '\0';
            key++;
        }
        
        // Find the field in the schema
        int field_index = get_field_index(writer->schema, key);
        if (field_index < 0) {
            fprintf(stderr, "Warning: Field '%s' not found in schema, ignoring\n", key);
        } else {
            // Parse the value according to the field type
            if (parse_json_value(value, writer->schema->fields[field_index].type, &values[field_index]) < 0) {
                fprintf(stderr, "Error: Invalid value '%s' for field '%s'\n", value, key);
                free(values);
                free(json_copy);
                return -1;
            }
        }
        
        pair = strtok(NULL, ",");
    }
    
    // Insert the row using existing function
    int result = writer_insert_row(writer, values, writer->schema->field_count);
    
    free(values);
    free(json_copy);
    return result;
}

// Open existing .fxdb file for appending
writer_t* writer_open(const char* filename) {
    if (!filename) {
        return NULL;
    }
    
    // First, try to open the file for reading to validate it exists and is valid
    FILE* read_file = fopen(filename, "rb");
    if (!read_file) {
        fprintf(stderr, "Error: Cannot open file '%s' for reading: %s\n", filename, strerror(errno));
        return NULL;
    }
    
    // Read and validate header
    fxdb_header_t header;
    if (fread(&header, sizeof(fxdb_header_t), 1, read_file) != 1) {
        fprintf(stderr, "Error: Cannot read header from '%s'\n", filename);
        fclose(read_file);
        return NULL;
    }
    
    // Validate magic number
    if (header.magic != FXDB_MAGIC_NUM) {
        fprintf(stderr, "Error: Invalid file format - not a FlexonDB file\n");
        fclose(read_file);
        return NULL;
    }
    
    // Load schema from file
    if (fseek(read_file, header.schema_offset, SEEK_SET) != 0) {
        fprintf(stderr, "Error: Cannot seek to schema in '%s'\n", filename);
        fclose(read_file);
        return NULL;
    }
    
    // Read schema metadata
    uint32_t field_count, row_size, schema_str_len;
    if (fread(&field_count, sizeof(uint32_t), 1, read_file) != 1 ||
        fread(&row_size, sizeof(uint32_t), 1, read_file) != 1 ||
        fread(&schema_str_len, sizeof(uint32_t), 1, read_file) != 1) {
        fprintf(stderr, "Error: Cannot read schema metadata from '%s'\n", filename);
        fclose(read_file);
        return NULL;
    }
    
    // Read schema string
    char* schema_str = malloc(schema_str_len + 1);
    if (!schema_str) {
        fclose(read_file);
        return NULL;
    }
    
    if (fread(schema_str, 1, schema_str_len, read_file) != schema_str_len) {
        fprintf(stderr, "Error: Cannot read schema string from '%s'\n", filename);
        free(schema_str);
        fclose(read_file);
        return NULL;
    }
    schema_str[schema_str_len] = '\0';
    
    // Parse the schema string to create schema object
    schema_t* schema = parse_schema(schema_str);
    free(schema_str);
    
    if (!schema) {
        fprintf(stderr, "Error: Cannot parse schema from '%s'\n", filename);
        fclose(read_file);
        return NULL;
    }
    
    // Validate that the parsed schema matches the file metadata
    if (schema->field_count != field_count || schema->row_size != row_size) {
        fprintf(stderr, "Error: Schema mismatch in '%s'\n", filename);
        free_schema(schema);
        fclose(read_file);
        return NULL;
    }
    
    fclose(read_file);
    
    // Now open the file for appending
    FILE* append_file = fopen(filename, "r+b");
    if (!append_file) {
        fprintf(stderr, "Error: Cannot open file '%s' for appending: %s\n", filename, strerror(errno));
        free_schema(schema);
        return NULL;
    }
    
    // Create writer structure
    writer_t* writer = malloc(sizeof(writer_t));
    if (!writer) {
        free_schema(schema);
        fclose(append_file);
        return NULL;
    }
    
    // Initialize writer for appending
    memset(writer, 0, sizeof(writer_t));
    writer->file = append_file;
    writer->schema = schema;
    writer->config = writer_default_config();
    writer->header = header;
    writer->total_rows = header.total_rows;
    writer->current_chunk = header.chunk_count;
    
    // Position file pointer at the end for appending
    if (fseek(writer->file, 0, SEEK_END) != 0) {
        fprintf(stderr, "Error: Cannot seek to end of '%s'\n", filename);
        writer_free(writer);
        return NULL;
    }
    
    // Allocate row buffer
    writer->row_buffer = malloc(writer->config.chunk_size * writer->schema->row_size);
    if (!writer->row_buffer) {
        writer_free(writer);
        return NULL;
    }
    
    return writer;
}

/* ============================================================================
 * Enhanced Database Operations Implementation
 * ============================================================================ */

/**
 * Create a new .fxdb database with enhanced configuration
 */
int fxdb_database_create(const char* filename, const schema_t* schema, const fxdb_create_config_t* config) {
    if (!filename || !schema) {
        return -1;
    }

    // Normalize filename to ensure .fxdb extension
    char* normalized_name = fxdb_normalize_filename(filename);
    if (!normalized_name) {
        return -1;
    }

    // Check if file already exists
    if (fxdb_database_exists(normalized_name)) {
        free(normalized_name);
        return -1; // File already exists
    }

    // Use default config if none provided
    fxdb_create_config_t default_config = {
        .chunk_size = DEFAULT_CHUNK_SIZE,
        .enable_compression = false,
        .enable_indexing = false,
        .enable_checksum = true,
        .initial_capacity = 0
    };
    
    if (!config) {
        config = &default_config;
    }

    // Create writer configuration
    writer_config_t writer_config = {
        .chunk_size = config->chunk_size,
        .use_compression = config->enable_compression,
        .build_index = config->enable_indexing
    };

    // Create the database using existing writer_create
    writer_t* writer = writer_create(normalized_name, schema, &writer_config);
    if (!writer) {
        free(normalized_name);
        return -1;
    }

    // Close immediately since we're just creating
    int result = writer_close(writer);
    writer_free(writer);
    
    free(normalized_name);
    return result;
}

/**
 * Open existing .fxdb file for appending (ENHANCED)
 */
writer_t* fxdb_writer_open(const char* filename, fxdb_open_mode_t mode) {
    if (!filename) {
        return NULL;
    }

    // Normalize filename
    char* normalized_name = fxdb_normalize_filename(filename);
    if (!normalized_name) {
        return NULL;
    }

    // Check if file exists
    bool file_exists = fxdb_database_exists(normalized_name);
    
    // Handle different open modes
    if ((mode & FXDB_OPEN_CREATE) && (mode & FXDB_OPEN_EXCLUSIVE) && file_exists) {
        free(normalized_name);
        return NULL; // File exists but exclusive creation requested
    }
    
    if (!(mode & FXDB_OPEN_CREATE) && !file_exists) {
        free(normalized_name);
        return NULL; // File doesn't exist and creation not requested
    }

    // For now, just call the original writer_open for append mode
    // TODO: Implement full mode support
    writer_t* writer = writer_open(normalized_name);
    
    free(normalized_name);
    return writer;
}