#include "../include/schema.h"
#include "../include/writer.h"
#include "../include/reader.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("=== FlexonDB Reader Module Test ===\n\n");
    
    // First, create a test database
    printf("Step 1: Creating test database...\n");
    
    schema_t* schema = parse_schema("name string, age int32, salary float, active bool");
    if (!schema) {
        printf("Failed to create schema!\n");
        return 1;
    }
    
    writer_t* writer = writer_create_default("test_read.fxdb", schema);
    if (!writer) {
        printf("Failed to create writer!\n");
        free_schema(schema);
        return 1;
    }
    
    // Insert test data
    field_value_t employees[] = {
        {"name", .value.string_val = "Alice Johnson"},
        {"age", .value.int32_val = 28},
        {"salary", .value.float_val = 75000.50f},
        {"active", .value.bool_val = true}
    };
    writer_insert_row(writer, employees, 4);
    
    field_value_t employees2[] = {
        {"name", .value.string_val = "Bob Smith"},
        {"age", .value.int32_val = 35},
        {"salary", .value.float_val = 82000.00f},
        {"active", .value.bool_val = false}
    };
    writer_insert_row(writer, employees2, 4);
    
    field_value_t employees3[] = {
        {"name", .value.string_val = "Carol Davis"},
        {"age", .value.int32_val = 42},
        {"salary", .value.float_val = 95000.25f},
        {"active", .value.bool_val = true}
    };
    writer_insert_row(writer, employees3, 4);
    
    writer_close(writer);
    writer_free(writer);
    free_schema(schema);
    
    printf("✓ Test database created with 3 rows\n\n");
    
    // Now test reading
    printf("Step 2: Testing reader...\n");
    
    reader_t* reader = reader_open("test_read.fxdb");
    if (!reader) {
        printf("Failed to open reader!\n");
        return 1;
    }
    
    printf("✓ Reader opened successfully\n\n");
    
    // Test info
    uint32_t total_rows, total_chunks;
    reader_get_stats(reader, &total_rows, &total_chunks);
    printf("Database stats: %u rows, %u chunks\n\n", total_rows, total_chunks);
    
    // Test reading all rows
    printf("Reading all rows:\n");
    query_result_t* result = reader_read_rows(reader, 10);
    if (result) {
        reader_print_rows(reader, result);
        reader_free_result(result);
    }
    
    reader_close(reader);
    
    printf("\n=== Reader Test Complete ===\n");
    return 0;
}