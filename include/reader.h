#ifndef READER_H
#define READER_H

#include "schema.h"
#include "writer.h"
#include <stdint.h>
#include <stdio.h>

// Reader context
typedef struct {
    FILE* file;                 // File handle
    schema_t* schema;           // Schema loaded from file
    fxdb_header_t header;       // File header
    
    // Current position
    uint32_t current_chunk;     // Current chunk being read
    uint32_t current_row;       // Current row in chunk
    uint32_t chunk_row_count;   // Rows in current chunk
    uint8_t* chunk_buffer;      // Buffer for current chunk
    long chunk_data_start;      // Start of current chunk data
} reader_t;

// Row data for reading
typedef struct {
    uint32_t field_count;
    field_value_t* values;
} row_data_t;

// Query result
typedef struct {
    uint32_t row_count;
    row_data_t* rows;
} query_result_t;

// Function declarations

/**
 * Open .fxdb file for reading
 * Returns reader_t pointer on success, NULL on failure
 */
reader_t* reader_open(const char* filename);

/**
 * Read next row from file
 * Returns row_data_t pointer on success, NULL on EOF or error
 */
row_data_t* reader_read_row(reader_t* reader);

/**
 * Read multiple rows with limit
 * Returns query_result_t pointer on success, NULL on error
 */
query_result_t* reader_read_rows(reader_t* reader, uint32_t limit);

/**
 * Seek to specific row number (0-based)
 * Returns 0 on success, -1 on error
 */
int reader_seek_row(reader_t* reader, uint32_t row_number);

/**
 * Get total row count
 */
uint32_t reader_get_row_count(const reader_t* reader);

/**
 * Get reader statistics
 */
void reader_get_stats(const reader_t* reader, uint32_t* total_rows, uint32_t* total_chunks);

/**
 * Print row data in formatted table
 */
void reader_print_row(const reader_t* reader, const row_data_t* row);

/**
 * Print multiple rows in formatted table
 */
void reader_print_rows(const reader_t* reader, const query_result_t* result);

/**
 * Close reader
 */
void reader_close(reader_t* reader);

/**
 * Free row data
 */
void reader_free_row(row_data_t* row);

/**
 * Free query result
 */
void reader_free_result(query_result_t* result);

// Helper functions

/**
 * Deserialize row from buffer
 */
row_data_t* deserialize_row(const schema_t* schema, const uint8_t* buffer);

/**
 * Load chunk at index
 */
int reader_load_chunk(reader_t* reader, uint32_t chunk_index);

#endif // READER_H