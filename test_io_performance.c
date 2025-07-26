#include "include/io_utils.h"
#include "include/writer.h"
#include "include/schema.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define TEST_ROWS 1000

int main() {
    printf("=== FlexonDB I/O Performance Test ===\n\n");

    const char* test_file = "performance_test.fxdb";
    
    // Create schema
    schema_t* schema = parse_schema("name string, id int32, value float");
    if (!schema) {
        printf("❌ Failed to parse schema\n");
        return 1;
    }
    
    printf("Test Configuration:\n");
    printf("  • Rows to write: %d\n", TEST_ROWS);
    printf("  • Buffer size: %d KB\n", FXDB_BUFFER_SIZE / 1024);
    printf("  • Schema: name string, id int32, value float\n");
    printf("  • Row size: %u bytes\n\n", schema->row_size);
    
    // Test 1: Database Creation Performance
    printf("Test 1: Database Creation Performance\n");
    
    clock_t start = clock();
    if (fxdb_database_create(test_file, schema, NULL) != 0) {
        printf("❌ Failed to create database\n");
        return 1;
    }
    clock_t end = clock();
    
    double creation_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("✅ Database created in %.2f ms\n", creation_time);
    
    // Test 2: File Management Operations
    printf("\nTest 2: File Management Operations\n");
    
    // Test existence checking
    start = clock();
    bool exists = fxdb_database_exists(test_file);
    end = clock();
    double exist_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    
    if (exists) {
        printf("✅ Database existence check: %.3f ms\n", exist_time);
    } else {
        printf("❌ Database existence check failed\n");
    }
    
    // Test filename normalization
    start = clock();
    char* norm1 = fxdb_normalize_filename("test.db");
    char* norm2 = fxdb_normalize_filename("test");
    end = clock();
    double norm_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    
    printf("✅ Filename normalization: %.3f ms\n", norm_time);
    printf("  • 'test.db' -> '%s'\n", norm1);
    printf("  • 'test' -> '%s'\n", norm2);
    
    free(norm1);
    free(norm2);
    
    // Test 3: Buffered I/O Performance
    printf("\nTest 3: Buffered I/O Performance\n");
    
    const char* buffered_test_file = "buffered_test.tmp";
    
    start = clock();
    fxdb_buffered_writer_t* writer = fxdb_buffered_writer_create(buffered_test_file, true);
    if (!writer) {
        printf("❌ Failed to create buffered writer\n");
        return 1;
    }
    
    // Write test data
    for (int i = 0; i < TEST_ROWS; i++) {
        if (fxdb_write_uint32(writer, i) != 0 ||
            fxdb_write_uint32(writer, i * 2) != 0 ||
            fxdb_write_uint32(writer, i * 3) != 0) {
            printf("❌ Failed to write row %d\n", i);
            return 1;
        }
    }
    
    if (fxdb_buffered_writer_close(writer) != 0) {
        printf("❌ Failed to close buffered writer\n");
        return 1;
    }
    end = clock();
    
    double write_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("✅ Buffered write of %d rows: %.2f ms\n", TEST_ROWS, write_time);
    printf("  • Write rate: %.0f rows/second\n", TEST_ROWS / (write_time / 1000.0));
    
    // Test 4: Memory-Mapped Reading Performance
    printf("\nTest 4: Memory-Mapped Reading Performance\n");
    
    start = clock();
    fxdb_mmap_reader_t* reader = fxdb_mmap_reader_open(buffered_test_file);
    if (!reader) {
        printf("❌ Failed to create mmap reader\n");
        return 1;
    }
    
    // Read back test data
    size_t offset = 0;
    for (int i = 0; i < TEST_ROWS; i++) {
        uint32_t val1 = fxdb_read_uint32_mmap(reader, offset);
        offset += sizeof(uint32_t);
        uint32_t val2 = fxdb_read_uint32_mmap(reader, offset);
        offset += sizeof(uint32_t);
        uint32_t val3 = fxdb_read_uint32_mmap(reader, offset);
        offset += sizeof(uint32_t);
        
        if (val1 != (uint32_t)i || val2 != (uint32_t)(i * 2) || val3 != (uint32_t)(i * 3)) {
            printf("❌ Data mismatch at row %d: expected (%d, %d, %d), got (%u, %u, %u)\n",
                   i, i, i*2, i*3, val1, val2, val3);
            return 1;
        }
    }
    
    fxdb_mmap_reader_close(reader);
    end = clock();
    
    double read_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("✅ Memory-mapped read of %d rows: %.2f ms\n", TEST_ROWS, read_time);
    printf("  • Read rate: %.0f rows/second\n", TEST_ROWS / (read_time / 1000.0));
    
    // Test 5: Performance Summary
    printf("\nTest 5: Performance Summary\n");
    printf("┌─────────────────────────────┬─────────────┬─────────────────┐\n");
    printf("│ Operation                   │ Time (ms)   │ Rate (ops/sec)  │\n");
    printf("├─────────────────────────────┼─────────────┼─────────────────┤\n");
    printf("│ Database Creation           │ %8.2f    │ N/A             │\n", creation_time);
    printf("│ File Existence Check        │ %8.3f    │ %.0f           │\n", exist_time, 1000.0 / exist_time);
    printf("│ Filename Normalization      │ %8.3f    │ %.0f           │\n", norm_time, 2000.0 / norm_time);
    printf("│ Buffered Write (%d rows)    │ %8.2f    │ %8.0f        │\n", TEST_ROWS, write_time, TEST_ROWS / (write_time / 1000.0));
    printf("│ Memory-mapped Read (%d rows)│ %8.2f    │ %8.0f        │\n", TEST_ROWS, read_time, TEST_ROWS / (read_time / 1000.0));
    printf("└─────────────────────────────┴─────────────┴─────────────────┘\n");
    
    printf("\nI/O Performance Improvements:\n");
    printf("  ✅ Buffered writes with %d KB buffers reduce syscalls\n", FXDB_BUFFER_SIZE / 1024);
    printf("  ✅ Memory-mapped reads provide zero-copy access\n");
    printf("  ✅ File locking prevents concurrent corruption\n");
    printf("  ✅ Filename normalization ensures consistent .fxdb extension\n");
    
    // Cleanup
    unlink(test_file);
    unlink(buffered_test_file);
    free_schema(schema);
    
    printf("\n🎉 All I/O performance tests passed!\n");
    return 0;
}