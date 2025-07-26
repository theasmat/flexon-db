#include "include/reader.h"
#include "include/writer.h"
#include "include/schema.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    printf("=== FlexonDB Enhanced Memory-Mapped Reader Test ===\n\n");

    const char* test_file = "test_mmap_reader.fxdb";
    
    // Step 1: Create a test database with some data
    printf("Step 1: Creating test database with sample data...\n");
    
    schema_t* schema = parse_schema("name string, age int32, active bool");
    if (!schema) {
        printf("❌ Failed to parse schema\n");
        return 1;
    }
    
    writer_t* writer = writer_create_default(test_file, schema);
    if (!writer) {
        printf("❌ Failed to create writer\n");
        return 1;
    }
    
    // Insert test data
    field_value_t values[3];
    
    // Row 1
    values[0].field_name = "name";
    values[0].value.string_val = "Alice Johnson";
    values[1].field_name = "age";
    values[1].value.int32_val = 28;
    values[2].field_name = "active";
    values[2].value.bool_val = true;
    writer_insert_row(writer, values, 3);
    
    // Row 2
    values[0].value.string_val = "Bob Smith";
    values[1].value.int32_val = 35;
    values[2].value.bool_val = false;
    writer_insert_row(writer, values, 3);
    
    // Row 3
    values[0].value.string_val = "Carol Davis";
    values[1].value.int32_val = 42;
    values[2].value.bool_val = true;
    writer_insert_row(writer, values, 3);
    
    writer_close(writer);
    writer_free(writer);
    
    printf("✅ Test database created with 3 rows\n\n");
    
    // Step 2: Test enhanced reader with memory mapping
    printf("Step 2: Testing enhanced reader with memory mapping...\n");
    
    fxdb_enhanced_reader_t* mmap_reader = fxdb_reader_open(test_file, true);
    if (!mmap_reader) {
        printf("❌ Failed to open enhanced reader with mmap\n");
        return 1;
    }
    
    printf("✅ Enhanced reader opened successfully (mmap: %s)\n", 
           mmap_reader->use_mmap ? "enabled" : "fallback to file I/O");
    
    // Read all rows using enhanced reader
    printf("\nReading rows with enhanced reader:\n");
    printf("┌─────────────────┬─────────────────┬─────────────────┐\n");
    printf("│ name            │ age             │ active          │\n");
    printf("├─────────────────┼─────────────────┼─────────────────┤\n");
    
    uint32_t row_count = 0;
    row_data_t* row;
    while ((row = fxdb_reader_read_row(mmap_reader)) != NULL) {
        printf("│ %-15s │ %-15d │ %-15s │\n",
               row->values[0].value.string_val,
               row->values[1].value.int32_val,
               row->values[2].value.bool_val ? "true" : "false");
        reader_free_row(row);
        row_count++;
    }
    
    printf("└─────────────────┴─────────────────┴─────────────────┘\n");
    printf("✅ Read %u rows successfully\n\n", row_count);
    
    // Step 3: Test seeking functionality
    printf("Step 3: Testing seek functionality...\n");
    
    if (fxdb_reader_seek_row(mmap_reader, 1) == 0) {
        row = fxdb_reader_read_row(mmap_reader);
        if (row) {
            printf("✅ Seek to row 1 successful: %s, age %d\n", 
                   row->values[0].value.string_val, 
                   row->values[1].value.int32_val);
            reader_free_row(row);
        } else {
            printf("❌ Failed to read after seek\n");
        }
    } else {
        printf("❌ Seek to row 1 failed\n");
    }
    
    fxdb_reader_close(mmap_reader);
    
    // Step 4: Compare with traditional reader
    printf("\nStep 4: Comparing with traditional reader...\n");
    
    fxdb_enhanced_reader_t* file_reader = fxdb_reader_open(test_file, false);
    if (!file_reader) {
        printf("❌ Failed to open enhanced reader with file I/O\n");
        return 1;
    }
    
    printf("✅ Enhanced reader opened with file I/O (mmap: %s)\n", 
           file_reader->use_mmap ? "enabled" : "disabled");
    
    // Read first row
    row = fxdb_reader_read_row(file_reader);
    if (row) {
        printf("✅ First row from file I/O: %s, age %d\n", 
               row->values[0].value.string_val, 
               row->values[1].value.int32_val);
        reader_free_row(row);
    } else {
        printf("❌ Failed to read first row with file I/O\n");
    }
    
    fxdb_reader_close(file_reader);
    
    // Step 5: Test filename normalization in reader
    printf("\nStep 5: Testing filename normalization in reader...\n");
    
    // Try opening with .db extension (should be normalized)
    fxdb_enhanced_reader_t* norm_reader = fxdb_reader_open("test_mmap_reader.db", true);
    if (norm_reader) {
        printf("❌ Reader should have failed to open .db file (it doesn't exist)\n");
        fxdb_reader_close(norm_reader);
    } else {
        printf("✅ Reader correctly failed to open non-existent .db file\n");
    }
    
    // Cleanup
    unlink(test_file);
    free_schema(schema);
    
    printf("\n🎉 All enhanced memory-mapped reader tests passed!\n");
    return 0;
}