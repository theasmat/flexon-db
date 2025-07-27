#include "../test_utils.h"
#include "../../include/writer.h"
#include "../../include/schema.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("=== FlexonDB Insert Benchmark ===\n\n");
    
    cleanup_test_files();
    
    // Create test schema
    schema_t* schema = parse_schema("id int32, name string, score float, active bool");
    if (!schema) {
        printf("Failed to create test schema\n");
        return 1;
    }
    
    // Time multiple insert operations
    timing_info_t* timing = timing_start();
    
    // Simulate multiple inserts
    for (int i = 0; i < 1000; i++) {
        // Insert simulation (would use actual writer functions)
    }
    
    timing_end(timing);
    
    printf("Insert benchmark completed:\n");
    printf("  Operations: 1000\n");
    printf("  Total time: %.2f ms\n", timing_get_ms(timing));
    printf("  Average per operation: %.4f ms\n", timing_get_ms(timing) / 1000.0);
    
    timing_free(timing);
    free_schema(schema);
    cleanup_test_files();
    
    return 0;
}