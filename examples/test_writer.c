#include "../include/schema.h"
#include "../include/writer.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("=== FlexonDB Writer Module Test ===\n\n");
    
    // Create schema
    schema_t* schema = parse_schema("name string, age int32, salary float, active bool");
    if (!schema) {
        printf("Failed to create schema!\n");
        return 1;
    }
    
    printf("Created schema:\n");
    print_schema(schema);
    printf("\n");
    
    // Create writer
    writer_t* writer = writer_create_default("test.fxdb", schema);
    if (!writer) {
        printf("Failed to create writer!\n");
        free_schema(schema);
        return 1;
    }
    
    printf("Created writer for test.fxdb\n\n");
    
    // Insert some test data
    printf("Inserting test data...\n");
    
    // Row 1: Alice
    field_value_t row1[] = {
        {"name", .value.string_val = "Alice Johnson"},
        {"age", .value.int32_val = 28},
        {"salary", .value.float_val = 75000.50f},
        {"active", .value.bool_val = true}
    };
    
    if (writer_insert_row(writer, row1, 4) != 0) {
        printf("Failed to insert row 1\n");
    } else {
        printf("  ✓ Inserted Alice Johnson\n");
    }
    
    // Row 2: Bob
    field_value_t row2[] = {
        {"name", .value.string_val = "Bob Smith"},
        {"age", .value.int32_val = 35},
        {"salary", .value.float_val = 82000.00f},
        {"active", .value.bool_val = false}
    };
    
    if (writer_insert_row(writer, row2, 4) != 0) {
        printf("Failed to insert row 2\n");
    } else {
        printf("  ✓ Inserted Bob Smith\n");
    }
    
    // Row 3: Carol
    field_value_t row3[] = {
        {"name", .value.string_val = "Carol Davis"},
        {"age", .value.int32_val = 42},
        {"salary", .value.float_val = 95000.25f},
        {"active", .value.bool_val = true}
    };
    
    if (writer_insert_row(writer, row3, 4) != 0) {
        printf("Failed to insert row 3\n");
    } else {
        printf("  ✓ Inserted Carol Davis\n");
    }
    
    // Get statistics
    uint32_t total_rows, chunks_written;
    writer_get_stats(writer, &total_rows, &chunks_written);
    printf("\nWriter Statistics:\n");
    printf("  Total rows: %u\n", total_rows);
    printf("  Chunks written: %u\n", chunks_written);
    
    // Close writer
    if (writer_close(writer) != 0) {
        printf("Failed to close writer!\n");
    } else {
        printf("\n✓ Writer closed successfully\n");
    }
    
    // Clean up
    writer_free(writer);
    free_schema(schema);
    
    // Check file size
    FILE* file = fopen("test.fxdb", "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fclose(file);
        printf("✓ Created test.fxdb (%ld bytes)\n", file_size);
    }
    
    printf("\n=== Writer Test Complete ===\n");
    return 0;
}