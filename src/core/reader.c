#include "../../include/reader.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Load schema from file
static schema_t* load_schema_from_file(FILE* file, const fxdb_header_t* header) {
    if (fseek(file, header->schema_offset, SEEK_SET) != 0) {
        return NULL;
    }
    
    // Read schema metadata
    uint32_t field_count, row_size, schema_str_len;
    if (fread(&field_count, sizeof(uint32_t), 1, file) != 1 ||
        fread(&row_size, sizeof(uint32_t), 1, file) != 1 ||
        fread(&schema_str_len, sizeof(uint32_t), 1, file) != 1) {
        return NULL;
    }
    
    // Read schema string
    char* schema_str = malloc(schema_str_len + 1);
    if (!schema_str) return NULL;
    
    if (fread(schema_str, 1, schema_str_len, file) != schema_str_len) {
        free(schema_str);
        return NULL;
    }
    schema_str[schema_str_len] = '\0';
    
    // Create schema structure
    schema_t* schema = malloc(sizeof(schema_t));
    if (!schema) {
        free(schema_str);
        return NULL;
    }
    
    schema->field_count = field_count;
    schema->row_size = row_size;
    schema->raw_schema_str = schema_str;
    
    // Read field definitions
    for (uint32_t i = 0; i < field_count; i++) {
        field_def_t* field = &schema->fields[i];
        
        if (fread(field->name, 1, MAX_FIELD_NAME_LEN, file) != MAX_FIELD_NAME_LEN ||
            fread(&field->type, sizeof(field_type_t), 1, file) != 1 ||
            fread(&field->size, sizeof(uint32_t), 1, file) != 1) {
            free_schema(schema);
            return NULL;
        }
    }
    
    return schema;
}

// Open .fxdb file for reading
reader_t* reader_open(const char* filename) {
    if (!filename) {
        return NULL;
    }
    
    reader_t* reader = malloc(sizeof(reader_t));
    if (!reader) {
        return NULL;
    }
    
    memset(reader, 0, sizeof(reader_t));
    
    // Open file for reading
    reader->file = fopen(filename, "rb");
    if (!reader->file) {
        fprintf(stderr, "Error: Cannot open file '%s': %s\n", filename, strerror(errno));
        free(reader);
        return NULL;
    }
    
    // Read header
    if (fread(&reader->header, sizeof(fxdb_header_t), 1, reader->file) != 1) {
        fprintf(stderr, "Error: Cannot read file header\n");
        fclose(reader->file);
        free(reader);
        return NULL;
    }
    
    // Validate magic number
    if (reader->header.magic != FXDB_MAGIC_NUM) {
        fprintf(stderr, "Error: Invalid file format (magic number mismatch)\n");
        fclose(reader->file);
        free(reader);
        return NULL;
    }
    
    // Check version
    if (reader->header.version != FXDB_VERSION) {
        fprintf(stderr, "Error: Unsupported file version %u\n", reader->header.version);
        fclose(reader->file);
        free(reader);
        return NULL;
    }
    
    // Load schema
    reader->schema = load_schema_from_file(reader->file, &reader->header);
    if (!reader->schema) {
        fprintf(stderr, "Error: Cannot load schema from file\n");
        fclose(reader->file);
        free(reader);
        return NULL;
    }
    
    // Allocate chunk buffer
    reader->chunk_buffer = malloc(reader->header.chunk_size * reader->schema->row_size);
    if (!reader->chunk_buffer) {
        free_schema(reader->schema);
        fclose(reader->file);
        free(reader);
        return NULL;
    }
    
    return reader;
}

// Load chunk at index
int reader_load_chunk(reader_t* reader, uint32_t chunk_index) {
    if (!reader || chunk_index >= reader->header.chunk_count) {
        return -1;
    }
    
    // Calculate chunk position
    long chunk_pos = reader->header.data_offset;
    
    // Skip to target chunk
    for (uint32_t i = 0; i < chunk_index; i++) {
        if (fseek(reader->file, chunk_pos, SEEK_SET) != 0) {
            return -1;
        }
        
        uint32_t chunk_header[2];
        if (fread(chunk_header, sizeof(uint32_t), 2, reader->file) != 2) {
            return -1;
        }
        
        chunk_pos += sizeof(chunk_header) + chunk_header[1]; // header + data size
    }
    
    // Read target chunk header
    if (fseek(reader->file, chunk_pos, SEEK_SET) != 0) {
        return -1;
    }
    
    uint32_t chunk_header[2];
    if (fread(chunk_header, sizeof(uint32_t), 2, reader->file) != 2) {
        return -1;
    }
    
    reader->chunk_row_count = chunk_header[0];
    uint32_t chunk_data_size = chunk_header[1];
    
    // Read chunk data
    if (fread(reader->chunk_buffer, 1, chunk_data_size, reader->file) != chunk_data_size) {
        return -1;
    }
    
    reader->current_chunk = chunk_index;
    reader->current_row = 0;
    reader->chunk_data_start = chunk_pos + sizeof(chunk_header);
    
    return 0;
}

// Deserialize row from buffer
row_data_t* deserialize_row(const schema_t* schema, const uint8_t* buffer) {
    if (!schema || !buffer) {
        return NULL;
    }
    
    row_data_t* row = malloc(sizeof(row_data_t));
    if (!row) return NULL;
    
    row->field_count = schema->field_count;
    row->values = malloc(sizeof(field_value_t) * schema->field_count);
    if (!row->values) {
        free(row);
        return NULL;
    }
    
    uint32_t offset = 0;
    
    for (uint32_t i = 0; i < schema->field_count; i++) {
        const field_def_t* field = &schema->fields[i];
        field_value_t* value = &row->values[i];
        
        value->field_name = field->name; // Point to schema field name
        
        switch (field->type) {
            case FIELD_TYPE_INT32:
                memcpy(&value->value.int32_val, buffer + offset, sizeof(int32_t));
                offset += sizeof(int32_t);
                break;
                
            case FIELD_TYPE_FLOAT:
                memcpy(&value->value.float_val, buffer + offset, sizeof(float));
                offset += sizeof(float);
                break;
                
            case FIELD_TYPE_BOOL:
                value->value.bool_val = (buffer[offset] != 0);
                offset += 1;
                break;
                
            case FIELD_TYPE_STRING: {
                // Allocate and copy string
                char* str = malloc(field->size);
                if (str) {
                    memcpy(str, buffer + offset, field->size);
                    str[field->size - 1] = '\0'; // Ensure null termination
                    value->value.string_val = str;
                } else {
                    value->value.string_val = NULL;
                }
                offset += field->size;
                break;
            }
                
            default:
                fprintf(stderr, "Error: Unknown field type %d\n", field->type);
                reader_free_row(row);
                return NULL;
        }
    }
    
    return row;
}

// Read next row from file
row_data_t* reader_read_row(reader_t* reader) {
    if (!reader) return NULL;
    
    // Load first chunk if needed
    if (reader->current_chunk == 0 && reader->current_row == 0 && reader->chunk_row_count == 0) {
        if (reader_load_chunk(reader, 0) != 0) {
            return NULL;
        }
    }
    
    // Check if we need to move to next chunk
    if (reader->current_row >= reader->chunk_row_count) {
        if (reader->current_chunk + 1 >= reader->header.chunk_count) {
            return NULL; // EOF
        }
        
        if (reader_load_chunk(reader, reader->current_chunk + 1) != 0) {
            return NULL;
        }
    }
    
    // Deserialize current row
    uint8_t* row_buffer = reader->chunk_buffer + (reader->current_row * reader->schema->row_size);
    row_data_t* row = deserialize_row(reader->schema, row_buffer);
    
    if (row) {
        reader->current_row++;
    }
    
    return row;
}

// Read multiple rows with limit
query_result_t* reader_read_rows(reader_t* reader, uint32_t limit) {
    if (!reader) return NULL;
    
    query_result_t* result = malloc(sizeof(query_result_t));
    if (!result) return NULL;
    
    result->row_count = 0;
    result->rows = malloc(sizeof(row_data_t) * limit);
    if (!result->rows) {
        free(result);
        return NULL;
    }
    
    for (uint32_t i = 0; i < limit; i++) {
        row_data_t* row = reader_read_row(reader);
        if (!row) break;
        
        result->rows[result->row_count] = *row;
        result->row_count++;
        free(row); // Free the container, but keep the data
    }
    
    return result;
}

// Get total row count
uint32_t reader_get_row_count(const reader_t* reader) {
    return reader ? reader->header.total_rows : 0;
}

// Get reader statistics
void reader_get_stats(const reader_t* reader, uint32_t* total_rows, uint32_t* total_chunks) {
    if (reader) {
        if (total_rows) *total_rows = reader->header.total_rows;
        if (total_chunks) *total_chunks = reader->header.chunk_count;
    }
}

// Print row data in formatted table
void reader_print_row(const reader_t* reader, const row_data_t* row) {
    if (!reader || !row) return;
    
    for (uint32_t i = 0; i < row->field_count; i++) {
        const field_value_t* value = &row->values[i];
        const field_def_t* field = &reader->schema->fields[i];
        
        printf("%-15s: ", field->name);
        
        switch (field->type) {
            case FIELD_TYPE_INT32:
                printf("%d\n", value->value.int32_val);
                break;
            case FIELD_TYPE_FLOAT:
                printf("%.2f\n", value->value.float_val);
                break;
            case FIELD_TYPE_BOOL:
                printf("%s\n", value->value.bool_val ? "true" : "false");
                break;
            case FIELD_TYPE_STRING:
                printf("%s\n", value->value.string_val ? value->value.string_val : "(null)");
                break;
            default:
                printf("(unknown)\n");
                break;
        }
    }
    printf("\n");
}

// Print multiple rows in formatted table
void reader_print_rows(const reader_t* reader, const query_result_t* result) {
    if (!reader || !result || result->row_count == 0) {
        printf("No rows to display.\n");
        return;
    }
    
    // Print header
    printf("┌");
    for (uint32_t i = 0; i < reader->schema->field_count; i++) {
        printf("─────────────────");
        if (i < reader->schema->field_count - 1) printf("┬");
    }
    printf("┐\n");
    
    printf("│");
    for (uint32_t i = 0; i < reader->schema->field_count; i++) {
        printf(" %-15s │", reader->schema->fields[i].name);
    }
    printf("\n");
    
    printf("├");
    for (uint32_t i = 0; i < reader->schema->field_count; i++) {
        printf("─────────────────");
        if (i < reader->schema->field_count - 1) printf("┼");
    }
    printf("┤\n");
    
    // Print rows
    for (uint32_t r = 0; r < result->row_count; r++) {
        const row_data_t* row = &result->rows[r];
        printf("│");
        
        for (uint32_t i = 0; i < row->field_count; i++) {
            const field_value_t* value = &row->values[i];
            const field_def_t* field = &reader->schema->fields[i];
            
            switch (field->type) {
                case FIELD_TYPE_INT32:
                    printf(" %-15d │", value->value.int32_val);
                    break;
                case FIELD_TYPE_FLOAT:
                    printf(" %-15.2f │", value->value.float_val);
                    break;
                case FIELD_TYPE_BOOL:
                    printf(" %-15s │", value->value.bool_val ? "true" : "false");
                    break;
                case FIELD_TYPE_STRING:
                    printf(" %-15s │", value->value.string_val ? value->value.string_val : "(null)");
                    break;
                default:
                    printf(" %-15s │", "(unknown)");
                    break;
            }
        }
        printf("\n");
    }
    
    printf("└");
    for (uint32_t i = 0; i < reader->schema->field_count; i++) {
        printf("─────────────────");
        if (i < reader->schema->field_count - 1) printf("┴");
    }
    printf("┘\n");
    
    printf("\n%u row(s) displayed.\n", result->row_count);
}

// Close reader
void reader_close(reader_t* reader) {
    if (reader) {
        if (reader->file) {
            fclose(reader->file);
        }
        if (reader->schema) {
            free_schema(reader->schema);
        }
        if (reader->chunk_buffer) {
            free(reader->chunk_buffer);
        }
        free(reader);
    }
}

// Free row data
void reader_free_row(row_data_t* row) {
    if (row) {
        if (row->values) {
            // Free string values
            for (uint32_t i = 0; i < row->field_count; i++) {
                if (row->values[i].value.string_val) {
                    // Only free if it's not pointing to schema field name
                    free((char*)row->values[i].value.string_val);
                }
            }
            free(row->values);
        }
        free(row);
    }
}

// Free query result
void reader_free_result(query_result_t* result) {
    if (result) {
        if (result->rows) {
            for (uint32_t i = 0; i < result->row_count; i++) {
                // Free string values in each row
                for (uint32_t j = 0; j < result->rows[i].field_count; j++) {
                    if (result->rows[i].values[j].value.string_val) {
                        free((char*)result->rows[i].values[j].value.string_val);
                    }
                }
                if (result->rows[i].values) {
                    free(result->rows[i].values);
                }
            }
            free(result->rows);
        }
        free(result);
    }
}

// Seek to specific row number (placeholder)
int reader_seek_row(reader_t* reader __attribute__((unused)), uint32_t row_number __attribute__((unused))) {
    // TODO: Implement efficient seeking
    fprintf(stderr, "reader_seek_row: Not yet implemented\n");
    return -1;
}