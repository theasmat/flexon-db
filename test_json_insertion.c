#include "include/schema.h"
#include "include/writer.h"
#include "include/reader.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("=== FlexonDB JSON Insertion Test ===\n\n");
    
    // Create a schema
    schema_t* schema = parse_schema("name string, age int32, salary float, active bool");
    if (!schema) {
        printf("❌ Failed to parse schema\n");
        return 1;
    }
    
    printf("✓ Schema created with %u fields\n", schema->field_count);
    
    // Create a new database
    writer_t* writer = writer_create_default("test_json.fxdb", schema);
    if (!writer) {
        printf("❌ Failed to create writer\n");
        free_schema(schema);
        return 1;
    }
    
    printf("✓ Database created: test_json.fxdb\n");
    
    // Test JSON insertion
    printf("\nTesting JSON insertion...\n");
    
    // Test 1: Simple JSON
    const char* json1 = "{\"name\": \"Alice Johnson\", \"age\": 30, \"salary\": 75000.50, \"active\": true}";
    if (writer_insert_json(writer, json1) == 0) {
        printf("✓ Inserted: %s\n", json1);
    } else {
        printf("❌ Failed to insert: %s\n", json1);
    }
    
    // Test 2: Different order and boolean false
    const char* json2 = "{\"active\": false, \"salary\": 82000.0, \"name\": \"Bob Smith\", \"age\": 35}";
    if (writer_insert_json(writer, json2) == 0) {
        printf("✓ Inserted: %s\n", json2);
    } else {
        printf("❌ Failed to insert: %s\n", json2);
    }
    
    // Test 3: Missing some fields (should use defaults)
    const char* json3 = "{\"name\": \"Carol Davis\", \"age\": 42}";
    if (writer_insert_json(writer, json3) == 0) {
        printf("✓ Inserted: %s\n", json3);
    } else {
        printf("❌ Failed to insert: %s\n", json3);
    }
    
    // Test 4: Invalid JSON (should fail)
    const char* json4 = "{\"name\": \"Invalid\", \"age\": \"not a number\"}";
    if (writer_insert_json(writer, json4) == 0) {
        printf("❌ Should have failed to insert: %s\n", json4);
    } else {
        printf("✓ Correctly rejected invalid JSON: %s\n", json4);
    }
    
    // Close writer
    writer_close(writer);
    writer_free(writer);
    printf("\n✓ Writer closed successfully\n");
    
    // Test reading the data back
    printf("\nTesting data reading...\n");
    reader_t* reader = reader_open("test_json.fxdb");
    if (!reader) {
        printf("❌ Failed to open reader\n");
        free_schema(schema);
        return 1;
    }
    
    printf("✓ Reader opened successfully\n");
    
    query_result_t* result = reader_read_rows(reader, 10);
    if (result) {
        printf("\nInserted data:\n");
        reader_print_rows(reader, result);
        reader_free_result(result);
    } else {
        printf("❌ Failed to read data\n");
    }
    
    reader_close(reader);
    free_schema(schema);
    
    printf("\n=== JSON Insertion Test Complete ===\n");
    return 0;
}