#include "include/schema.h"
#include "include/writer.h"
#include "include/reader.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("=== FlexonDB Writer Open Test ===\n\n");
    
    // First, create a database with some initial data
    schema_t* schema = parse_schema("name string, age int32, department string");
    if (!schema) {
        printf("❌ Failed to parse schema\n");
        return 1;
    }
    
    printf("✓ Schema created\n");
    
    // Create initial database
    writer_t* writer1 = writer_create_default("test_append.fxdb", schema);
    if (!writer1) {
        printf("❌ Failed to create initial writer\n");
        free_schema(schema);
        return 1;
    }
    
    printf("✓ Initial database created: test_append.fxdb\n");
    
    // Insert some initial data
    writer_insert_json(writer1, "{\"name\": \"Alice\", \"age\": 30, \"department\": \"Engineering\"}");
    writer_insert_json(writer1, "{\"name\": \"Bob\", \"age\": 25, \"department\": \"Marketing\"}");
    
    printf("✓ Inserted 2 initial rows\n");
    
    // Close the first writer
    writer_close(writer1);
    writer_free(writer1);
    
    printf("✓ Initial writer closed\n");
    
    // Verify initial data
    printf("\nReading initial data...\n");
    reader_t* reader1 = reader_open("test_append.fxdb");
    if (reader1) {
        query_result_t* result1 = reader_read_rows(reader1, 10);
        if (result1) {
            printf("Initial data:\n");
            reader_print_rows(reader1, result1);
            reader_free_result(result1);
        }
        reader_close(reader1);
    }
    
    // Now test opening for appending
    printf("\nTesting writer_open for appending...\n");
    
    writer_t* writer2 = writer_open("test_append.fxdb");
    if (!writer2) {
        printf("❌ Failed to open database for appending\n");
        free_schema(schema);
        return 1;
    }
    
    printf("✓ Database opened for appending\n");
    
    // Insert additional data
    writer_insert_json(writer2, "{\"name\": \"Carol\", \"age\": 35, \"department\": \"Sales\"}");
    writer_insert_json(writer2, "{\"name\": \"David\", \"age\": 28, \"department\": \"Support\"}");
    
    printf("✓ Inserted 2 additional rows via append\n");
    
    // Close the append writer
    writer_close(writer2);
    writer_free(writer2);
    
    printf("✓ Append writer closed\n");
    
    // Verify all data is present
    printf("\nReading final data after append...\n");
    reader_t* reader2 = reader_open("test_append.fxdb");
    if (!reader2) {
        printf("❌ Failed to open reader for final verification\n");
        free_schema(schema);
        return 1;
    }
    
    query_result_t* result2 = reader_read_rows(reader2, 10);
    if (result2) {
        printf("Final data (should have 4 rows):\n");
        reader_print_rows(reader2, result2);
        reader_free_result(result2);
    } else {
        printf("❌ Failed to read final data\n");
    }
    
    reader_close(reader2);
    
    // Test opening non-existent file (should fail)
    printf("\nTesting opening non-existent file...\n");
    writer_t* writer3 = writer_open("nonexistent.fxdb");
    if (writer3) {
        printf("❌ Should have failed to open non-existent file\n");
        writer_free(writer3);
    } else {
        printf("✓ Correctly failed to open non-existent file\n");
    }
    
    free_schema(schema);
    
    printf("\n=== Writer Open Test Complete ===\n");
    return 0;
}