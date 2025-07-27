# FlexonDB API Reference

This document provides comprehensive API documentation for FlexonDB core functions.

## Table of Contents

- [Schema Operations](#schema-operations)
- [Writer Operations](#writer-operations)
- [Reader Operations](#reader-operations)
- [Data Types and Structures](#data-types-and-structures)
- [Error Handling](#error-handling)
- [Examples](#examples)

## Schema Operations

### `parse_schema()`

Parse a schema string into a schema structure.

```c
schema_t* parse_schema(const char* schema_str);
```

**Parameters:**
- `schema_str` - Schema definition string (e.g., "name string, age int32")

**Returns:**
- `schema_t*` - Parsed schema object, or `NULL` on error

**Example:**
```c
schema_t* schema = parse_schema("name string, age int32, salary float, active bool");
if (!schema) {
    fprintf(stderr, "Failed to parse schema\n");
    return -1;
}
```

### `free_schema()`

Free memory allocated for a schema.

```c
void free_schema(schema_t* schema);
```

**Parameters:**
- `schema` - Schema object to free

**Example:**
```c
free_schema(schema);
```

### `load_schema()`

Load schema from an existing .fxdb file.

```c
schema_t* load_schema(const char* filename);
```

**Parameters:**
- `filename` - Path to .fxdb file

**Returns:**
- `schema_t*` - Loaded schema object, or `NULL` on error

**Example:**
```c
schema_t* schema = load_schema("employees.fxdb");
if (schema) {
    print_schema(schema);
    free_schema(schema);
}
```

### `save_schema()`

Save schema to an existing .fxdb file header.

```c
int save_schema(const char* filename, const schema_t* schema);
```

**Parameters:**
- `filename` - Path to .fxdb file
- `schema` - Schema object to save

**Returns:**
- `0` on success, `-1` on error

### `print_schema()`

Print schema information to stdout.

```c
void print_schema(const schema_t* schema);
```

**Parameters:**
- `schema` - Schema object to print

## Writer Operations

### `writer_create_default()`

Create a new database file with default configuration.

```c
writer_t* writer_create_default(const char* filename, const schema_t* schema);
```

**Parameters:**
- `filename` - Database filename (will be normalized to .fxdb)
- `schema` - Database schema

**Returns:**
- `writer_t*` - Writer object, or `NULL` on error

**Example:**
```c
schema_t* schema = parse_schema("name string, age int32");
writer_t* writer = writer_create_default("users.fxdb", schema);
if (!writer) {
    fprintf(stderr, "Failed to create database\n");
    return -1;
}
```

### `writer_open()`

Open existing database for appending data.

```c
writer_t* writer_open(const char* filename);
```

**Parameters:**
- `filename` - Path to existing .fxdb file

**Returns:**
- `writer_t*` - Writer object, or `NULL` on error

**Example:**
```c
writer_t* writer = writer_open("existing.fxdb");
if (writer) {
    // Add more data to existing database
    writer_insert_json(writer, "{\"name\": \"New User\", \"age\": 25}");
    writer_close(writer);
    writer_free(writer);
}
```

### `writer_insert_json()`

Insert a row from JSON string.

```c
int writer_insert_json(writer_t* writer, const char* json_str);
```

**Parameters:**
- `writer` - Writer object
- `json_str` - JSON string with field values

**Returns:**
- `0` on success, `-1` on error

**Example:**
```c
int result = writer_insert_json(writer, "{\"name\": \"Alice\", \"age\": 30}");
if (result != 0) {
    fprintf(stderr, "Failed to insert data\n");
}
```

### `writer_insert_row()`

Insert a row using field values array.

```c
int writer_insert_row(writer_t* writer, const field_value_t* values, uint32_t value_count);
```

**Parameters:**
- `writer` - Writer object
- `values` - Array of field values
- `value_count` - Number of values

**Returns:**
- `0` on success, `-1` on error

**Example:**
```c
field_value_t values[2];
values[0].field_name = "name";
values[0].value.string_val = "Bob";
values[1].field_name = "age"; 
values[1].value.int32_val = 25;

int result = writer_insert_row(writer, values, 2);
```

### `writer_close()`

Close writer and finalize file.

```c
int writer_close(writer_t* writer);
```

**Parameters:**
- `writer` - Writer object

**Returns:**
- `0` on success, `-1` on error

### `writer_free()`

Free writer resources.

```c
void writer_free(writer_t* writer);
```

**Parameters:**
- `writer` - Writer object to free

**Example:**
```c
writer_close(writer);
writer_free(writer);
```

## Reader Operations

### `reader_open()`

Open database file for reading.

```c
reader_t* reader_open(const char* filename);
```

**Parameters:**
- `filename` - Path to .fxdb file

**Returns:**
- `reader_t*` - Reader object, or `NULL` on error

**Example:**
```c
reader_t* reader = reader_open("users.fxdb");
if (!reader) {
    fprintf(stderr, "Failed to open database\n");
    return -1;
}
```

### `reader_read_rows()`

Read multiple rows with optional limit.

```c
query_result_t* reader_read_rows(reader_t* reader, uint32_t limit);
```

**Parameters:**
- `reader` - Reader object
- `limit` - Maximum number of rows to read (0 = all)

**Returns:**
- `query_result_t*` - Query result, or `NULL` on error

**Example:**
```c
query_result_t* result = reader_read_rows(reader, 10);
if (result) {
    reader_print_rows(reader, result);
    reader_free_result(result);
}
```

### `reader_seek_row()`

Seek to specific row number.

```c
int reader_seek_row(reader_t* reader, uint32_t row_number);
```

**Parameters:**
- `reader` - Reader object
- `row_number` - Target row number (0-based)

**Returns:**
- `0` on success, `-1` on error

**Example:**
```c
// Seek to row 100
if (reader_seek_row(reader, 100) == 0) {
    query_result_t* result = reader_read_rows(reader, 1);
    // Process single row at position 100
}
```

### `reader_get_row_count()`

Get total number of rows in database.

```c
uint32_t reader_get_row_count(const reader_t* reader);
```

**Parameters:**
- `reader` - Reader object

**Returns:**
- Number of rows in database

### `reader_print_rows()`

Print rows in formatted table.

```c
void reader_print_rows(const reader_t* reader, const query_result_t* result);
```

**Parameters:**
- `reader` - Reader object
- `result` - Query result to print

### `reader_close()`

Close reader and free resources.

```c
void reader_close(reader_t* reader);
```

**Parameters:**
- `reader` - Reader object

### `reader_free_result()`

Free query result.

```c
void reader_free_result(query_result_t* result);
```

**Parameters:**
- `result` - Query result to free

**Example:**
```c
query_result_t* result = reader_read_rows(reader, 10);
if (result) {
    // Process result...
    reader_free_result(result);
}
reader_close(reader);
```

## Data Types and Structures

### `field_type_t`

Enumeration of supported field types.

```c
typedef enum {
    TYPE_INT32,
    TYPE_FLOAT, 
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_UNKNOWN
} field_type_t;
```

### `field_value_t`

Structure for field values.

```c
typedef struct {
    const char* field_name;
    union {
        int32_t int32_val;
        float float_val;
        const char* string_val;
        bool bool_val;
    } value;
} field_value_t;
```

### `schema_t`

Database schema structure.

```c
typedef struct {
    uint32_t field_count;
    uint32_t row_size;
    field_def_t fields[MAX_COLUMNS];
    char* raw_schema_str;
} schema_t;
```

### `query_result_t`

Query result structure.

```c
typedef struct {
    uint32_t row_count;
    row_data_t* rows;
    const schema_t* schema;
} query_result_t;
```

## Error Handling

All FlexonDB functions follow consistent error handling patterns:

### Return Values
- **Success**: `0` for functions returning int, valid pointer for functions returning pointers
- **Error**: `-1` for functions returning int, `NULL` for functions returning pointers

### Error Messages
FlexonDB prints descriptive error messages to stderr. Example:

```c
writer_t* writer = writer_open("nonexistent.fxdb");
if (!writer) {
    // Error message printed: "Error: Cannot open file 'nonexistent.fxdb': No such file or directory"
    return -1;
}
```

### Best Practices

1. **Always check return values**:
```c
if (writer_insert_json(writer, json) != 0) {
    fprintf(stderr, "Insert failed\n");
    return -1;
}
```

2. **Free resources properly**:
```c
if (result) {
    reader_free_result(result);
}
if (reader) {
    reader_close(reader);
}
if (schema) {
    free_schema(schema);
}
```

3. **Validate inputs**:
```c
if (!filename || !schema) {
    fprintf(stderr, "Invalid parameters\n");
    return -1;
}
```

## Examples

### Complete Database Workflow

```c
#include "flexondb.h"

int main() {
    // 1. Create schema
    schema_t* schema = parse_schema("name string, age int32, salary float");
    if (!schema) {
        fprintf(stderr, "Failed to parse schema\n");
        return 1;
    }
    
    // 2. Create database
    writer_t* writer = writer_create_default("employees.fxdb", schema);
    if (!writer) {
        fprintf(stderr, "Failed to create database\n");
        free_schema(schema);
        return 1;
    }
    
    // 3. Insert data
    writer_insert_json(writer, "{\"name\": \"Alice\", \"age\": 30, \"salary\": 75000.0}");
    writer_insert_json(writer, "{\"name\": \"Bob\", \"age\": 25, \"salary\": 65000.0}");
    
    // 4. Close writer
    writer_close(writer);
    writer_free(writer);
    
    // 5. Read data back
    reader_t* reader = reader_open("employees.fxdb");
    if (reader) {
        printf("Total rows: %u\n", reader_get_row_count(reader));
        
        query_result_t* result = reader_read_rows(reader, 0); // Read all
        if (result) {
            reader_print_rows(reader, result);
            reader_free_result(result);
        }
        
        reader_close(reader);
    }
    
    // 6. Cleanup
    free_schema(schema);
    return 0;
}
```

### Appending to Existing Database

```c
// Open existing database for appending
writer_t* writer = writer_open("employees.fxdb");
if (writer) {
    // Add new employees
    writer_insert_json(writer, "{\"name\": \"Carol\", \"age\": 35, \"salary\": 85000.0}");
    writer_insert_json(writer, "{\"name\": \"David\", \"age\": 28, \"salary\": 70000.0}");
    
    writer_close(writer);
    writer_free(writer);
}
```

### Error Handling Example

```c
int safe_insert_data(const char* filename, const char* json_data) {
    writer_t* writer = writer_open(filename);
    if (!writer) {
        fprintf(stderr, "Cannot open database: %s\n", filename);
        return -1;
    }
    
    if (writer_insert_json(writer, json_data) != 0) {
        fprintf(stderr, "Failed to insert data: %s\n", json_data);
        writer_close(writer);
        writer_free(writer);
        return -1;
    }
    
    if (writer_close(writer) != 0) {
        fprintf(stderr, "Failed to close database\n");
        writer_free(writer);
        return -1;
    }
    
    writer_free(writer);
    return 0;
}
```

---

For more examples and advanced usage, see the [examples](../examples/) directory and [EXAMPLES.md](EXAMPLES.md).