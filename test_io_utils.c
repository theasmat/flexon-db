#include "include/io_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    printf("=== FlexonDB I/O Utilities Test ===\n\n");

    const char* test_file = "test_io_utils.tmp";
    
    // Test 1: Buffered Writer
    printf("Test 1: Buffered Writer\n");
    fxdb_buffered_writer_t* writer = fxdb_buffered_writer_create(test_file, true);
    if (!writer) {
        printf("❌ Failed to create buffered writer\n");
        return 1;
    }
    
    // Write some test data
    uint32_t test_values[] = {0x12345678, 0xABCDEF00, 0xDEADBEEF};
    for (int i = 0; i < 3; i++) {
        if (fxdb_write_uint32(writer, test_values[i]) != 0) {
            printf("❌ Failed to write uint32 %d\n", i);
            return 1;
        }
    }
    
    // Write test strings
    const char* test_strings[] = {"Hello", "World", "FlexonDB"};
    for (int i = 0; i < 3; i++) {
        if (fxdb_write_string(writer, test_strings[i], 256) != 0) {
            printf("❌ Failed to write string %d\n", i);
            return 1;
        }
    }
    
    // Close writer
    if (fxdb_buffered_writer_close(writer) != 0) {
        printf("❌ Failed to close writer\n");
        return 1;
    }
    
    printf("✅ Buffered writer test passed\n\n");
    
    // Test 2: Memory-Mapped Reader
    printf("Test 2: Memory-Mapped Reader\n");
    fxdb_mmap_reader_t* reader = fxdb_mmap_reader_open(test_file);
    if (!reader) {
        printf("❌ Failed to create mmap reader\n");
        return 1;
    }
    
    // Read back the uint32 values
    size_t offset = 0;
    for (int i = 0; i < 3; i++) {
        uint32_t value = fxdb_read_uint32_mmap(reader, offset);
        if (value != test_values[i]) {
            printf("❌ Mismatch in uint32 %d: expected 0x%08X, got 0x%08X\n", 
                   i, test_values[i], value);
            return 1;
        }
        offset += sizeof(uint32_t);
    }
    
    // Read back the strings
    for (int i = 0; i < 3; i++) {
        char buffer[257];
        int len = fxdb_read_string_mmap(reader, offset, buffer, sizeof(buffer));
        if (len < 0 || strcmp(buffer, test_strings[i]) != 0) {
            printf("❌ Mismatch in string %d: expected '%s', got '%s'\n", 
                   i, test_strings[i], buffer);
            return 1;
        }
        offset += sizeof(uint32_t) + strlen(test_strings[i]); // length prefix + string data
    }
    
    fxdb_mmap_reader_close(reader);
    printf("✅ Memory-mapped reader test passed\n\n");
    
    // Test 3: File Management Functions
    printf("Test 3: File Management Functions\n");
    
    // Test filename normalization
    char* norm1 = fxdb_normalize_filename("test.db");
    char* norm2 = fxdb_normalize_filename("test");
    char* norm3 = fxdb_normalize_filename("test.fxdb");
    
    if (!norm1 || strcmp(norm1, "test.fxdb") != 0) {
        printf("❌ Failed to normalize 'test.db' -> expected 'test.fxdb', got '%s'\n", norm1);
        return 1;
    }
    
    if (!norm2 || strcmp(norm2, "test.fxdb") != 0) {
        printf("❌ Failed to normalize 'test' -> expected 'test.fxdb', got '%s'\n", norm2);
        return 1;
    }
    
    if (!norm3 || strcmp(norm3, "test.fxdb") != 0) {
        printf("❌ Failed to normalize 'test.fxdb' -> expected 'test.fxdb', got '%s'\n", norm3);
        return 1;
    }
    
    free(norm1);
    free(norm2);
    free(norm3);
    
    // Test file existence
    if (!fxdb_database_exists(test_file)) {
        printf("❌ File existence check failed\n");
        return 1;
    }
    
    printf("✅ File management test passed\n\n");
    
    // Cleanup
    unlink(test_file);
    
    printf("🎉 All I/O utilities tests passed!\n");
    return 0;
}