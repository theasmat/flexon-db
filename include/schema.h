#ifndef SCHEMA_H
#define SCHEMA_H

#include <stdint.h>
#include <stdbool.h>

// Maximum field name length
#define MAX_FIELD_NAME_LEN 64
#define MAX_SCHEMA_FIELDS 256

// Data types supported by FlexonDB
typedef enum {
    FIELD_TYPE_INT32,
    FIELD_TYPE_FLOAT,
    FIELD_TYPE_STRING,
    FIELD_TYPE_BOOL,
    FIELD_TYPE_UNKNOWN
} field_type_t;

// Field definition structure
typedef struct {
    char name[MAX_FIELD_NAME_LEN];
    field_type_t type;
    uint32_t size;  // Size in bytes (for strings: max length, others: fixed size)
} field_def_t;

// Schema structure
typedef struct {
    uint32_t field_count;
    uint32_t row_size;      // Total size of one row in bytes
    field_def_t fields[MAX_SCHEMA_FIELDS];
    char* raw_schema_str;   // Original schema string for reference
} schema_t;

// Function declarations

/**
 * Parse a schema string like "name string, age int32, salary float"
 * Returns pointer to schema_t on success, NULL on failure
 */
schema_t* parse_schema(const char* schema_str);

/**
 * Load schema from .fxdb file
 * Returns pointer to schema_t on success, NULL on failure
 */
schema_t* load_schema(const char* filename);

/**
 * Save schema to .fxdb file header
 * Returns 0 on success, -1 on failure
 */
int save_schema(const char* filename, const schema_t* schema);

/**
 * Print schema information to stdout
 */
void print_schema(const schema_t* schema);

/**
 * Free memory allocated for schema
 */
void free_schema(schema_t* schema);

/**
 * Get field index by name
 * Returns field index on success, -1 if not found
 */
int get_field_index(const schema_t* schema, const char* field_name);

/**
 * Calculate row size based on schema fields
 * Returns total row size in bytes
 */
uint32_t calculate_row_size(const schema_t* schema);

/**
 * Validate schema (check for duplicate field names, valid types, etc.)
 * Returns true if valid, false otherwise
 */
bool validate_schema(const schema_t* schema);

/**
 * Get string representation of field type
 */
const char* field_type_to_string(field_type_t type);

/**
 * Parse field type from string
 */
field_type_t string_to_field_type(const char* type_str);

#endif // SCHEMA_H