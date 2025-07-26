#include "../include/writer.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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

// Simple JSON parser for insert (basic implementation)
int writer_insert_json(writer_t* writer, const char* json_str __attribute__((unused))) {
    // TODO: Implement a simple JSON parser
    // For now, return error
    fprintf(stderr, "writer_insert_json: Not yet implemented\n");
    return -1;
}

// Open existing .fxdb file for appending
writer_t* writer_open(const char* filename __attribute__((unused))) {
    // TODO: Implement opening existing files
    // This requires reading the header and schema first
    fprintf(stderr, "writer_open: Not yet implemented\n");
    return NULL;
}