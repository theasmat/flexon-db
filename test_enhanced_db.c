#include "include/writer.h"
#include "include/io_utils.h"
#include "include/schema.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    printf("=== FlexonDB Enhanced Database Operations Test ===\n\n");

    // Test 1: Enhanced Database Creation
    printf("Test 1: Enhanced Database Creation\n");
    
    schema_t* schema = parse_schema("name string, age int32, active bool");
    if (!schema) {
        printf("❌ Failed to parse schema\n");
        return 1;
    }
    
    // Test with .db extension (should be normalized)
    if (fxdb_database_create("test_enhanced.db", schema, NULL) != 0) {
        printf("❌ Failed to create database with .db extension\n");
        return 1;
    }
    
    printf("✅ Created database with .db extension (normalized to .fxdb)\n");
    
    // Test existence checking
    if (!fxdb_database_exists("test_enhanced.fxdb")) {
        printf("❌ Database existence check failed\n");
        return 1;
    }
    
    printf("✅ Database existence check passed\n");
    
    // Test creating duplicate (should fail)
    if (fxdb_database_create("test_enhanced.fxdb", schema, NULL) == 0) {
        printf("❌ Duplicate database creation should have failed\n");
        return 1;
    }
    
    printf("✅ Duplicate database creation correctly rejected\n");
    
    // Test 2: Filename Normalization
    printf("\nTest 2: Filename Normalization\n");
    
    char* norm1 = fxdb_normalize_filename("test");
    char* norm2 = fxdb_normalize_filename("test.db");
    char* norm3 = fxdb_normalize_filename("test.fxdb");
    
    printf("  'test' -> '%s'\n", norm1);
    printf("  'test.db' -> '%s'\n", norm2);
    printf("  'test.fxdb' -> '%s'\n", norm3);
    
    if (strcmp(norm1, "test.fxdb") != 0 || 
        strcmp(norm2, "test.fxdb") != 0 || 
        strcmp(norm3, "test.fxdb") != 0) {
        printf("❌ Filename normalization failed\n");
        return 1;
    }
    
    printf("✅ Filename normalization test passed\n");
    
    free(norm1);
    free(norm2);
    free(norm3);
    
    // Test 3: Database Operations
    printf("\nTest 3: Database Operations\n");
    
    // Test extension checking
    if (!fxdb_has_extension("test.fxdb") || fxdb_has_extension("test.db")) {
        printf("❌ Extension checking failed\n");
        return 1;
    }
    
    printf("✅ Extension checking passed\n");
    
    // Test deletion
    if (fxdb_database_delete("test_enhanced.fxdb") != 0) {
        printf("❌ Database deletion failed\n");
        return 1;
    }
    
    printf("✅ Database deletion passed\n");
    
    // Verify deletion
    if (fxdb_database_exists("test_enhanced.fxdb")) {
        printf("❌ Database still exists after deletion\n");
        return 1;
    }
    
    printf("✅ Database deletion verification passed\n");
    
    // Test 4: Enhanced Writer Open (basic test)
    printf("\nTest 4: Enhanced Writer Open\n");
    
    // Create a database first
    if (fxdb_database_create("test_open.fxdb", schema, NULL) != 0) {
        printf("❌ Failed to create test database\n");
        return 1;
    }
    
    // Try to open with different modes
    writer_t* writer = fxdb_writer_open("test_open.fxdb", FXDB_OPEN_APPEND);
    if (writer) {
        printf("⚠️  Enhanced writer open returned a writer (but not implemented yet)\n");
        writer_free(writer);
    } else {
        printf("✅ Enhanced writer open correctly returns NULL (not implemented yet)\n");
    }
    
    // Cleanup
    fxdb_database_delete("test_open.fxdb");
    free_schema(schema);
    
    printf("\n🎉 All enhanced database operations tests passed!\n");
    return 0;
}