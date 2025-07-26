#ifndef WRITER_H
#define WRITER_H

#include "schema.h"
#include <stdint.h>
#include <stdio.h>

// FlexonDB file magic number "FXDB"
#define FXDB_MAGIC 0x42445846

// Current file format version
#define FXDB_VERSION 1

// Default chunk size (rows per chunk)
#define DEFAULT_CHUNK_SIZE 10000

// Writer configuration
typedef struct {
    uint32_t chunk_size;        // Rows per chunk
    bool use_compression;       // Enable compression (future)
    bool build_index;           // Build index while writing
} writer_config_t;

// FlexonDB file header structure
typedef struct {
    uint32_t magic;             // FXDB magic number
    uint32_t version;           // File format version
    uint32_t schema_offset;     // Offset to schema section
    uint32_t schema_size;       // Size of schema section
    uint32_t data_offset;       // Offset to data section
    uint32_t data_size;         // Size of data section
    uint32_t index_offset;      // Offset to index section (0 if no index)
    uint32_t index_size;        // Size of index section
    uint32_t total_rows;        // Total number of rows
    uint32_t chunk_size;        // Rows per chunk
    uint32_t chunk_count;       // Number of chunks
    uint8_t reserved[44];       // Reserved for future use (total header = 88 bytes)
} __attribute__((packed)) fxdb_header_t;

// Writer context
typedef struct {
    FILE* file;                 // File handle
    schema_t* schema;           // Schema definition
    writer_config_t config;     // Writer configuration
    fxdb_header_t header;       // File header
    
    // Write buffer
    uint8_t* row_buffer;        // Buffer for current chunk
    uint32_t buffer_row_count;  // Rows in buffer
    uint32_t total_rows;        // Total rows written
    uint32_t current_chunk;     // Current chunk number
    
    // File positions
    long schema_pos;            // Position where schema was written
    long data_start_pos;        // Position where data section starts
} writer_t;

// Row data structure for inserting
typedef struct {
    const char* field_name;
    union {
        int32_t int32_val;
        float float_val;
        const char* string_val;
        bool bool_val;
    } value;
} field_value_t;

// Function declarations

/**
 * Create a new .fxdb file with schema
 * Returns writer_t pointer on success, NULL on failure
 */
writer_t* writer_create(const char* filename, const schema_t* schema, const writer_config_t* config);

/**
 * Create writer with default configuration
 */
writer_t* writer_create_default(const char* filename, const schema_t* schema);

/**
 * Open existing .fxdb file for appending
 * Returns writer_t pointer on success, NULL on failure
 */
writer_t* writer_open(const char* filename);

/**
 * Insert a row using field values array
 * Returns 0 on success, -1 on failure
 */
int writer_insert_row(writer_t* writer, const field_value_t* values, uint32_t value_count);

/**
 * Insert a row from JSON string (simple parser)
 * Returns 0 on success, -1 on failure
 */
int writer_insert_json(writer_t* writer, const char* json_str);

/**
 * Flush current chunk to disk
 * Returns 0 on success, -1 on failure
 */
int writer_flush_chunk(writer_t* writer);

/**
 * Get writer statistics
 */
void writer_get_stats(const writer_t* writer, uint32_t* total_rows, uint32_t* chunks_written);

/**
 * Close writer and finalize file
 * Returns 0 on success, -1 on failure
 */
int writer_close(writer_t* writer);

/**
 * Free writer resources
 */
void writer_free(writer_t* writer);

// Helper functions

/**
 * Create default writer configuration
 */
writer_config_t writer_default_config(void);

/**
 * Serialize row data into buffer
 * Returns bytes written, or -1 on error
 */
int serialize_row(const schema_t* schema, const field_value_t* values, uint32_t value_count, uint8_t* buffer);

#endif // WRITER_H