#include "../test_utils.h"
#include "../../include/config.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    test_init("Data Types System Tests");
    
    // Test 1: Basic data type validation
    printf("Test 1: Current data types\n");
    test_assert(TYPE_INT32 == 0, "INT32 type value");
    test_assert(TYPE_FLOAT == 1, "FLOAT type value");
    test_assert(TYPE_STRING == 2, "STRING type value");
    test_assert(TYPE_BOOL == 3, "BOOL type value");
    test_assert(TYPE_UNKNOWN == 4, "UNKNOWN type value");
    
    // Note: Extended types will be tested here once implemented
    printf("Note: Extended data types tests will be added in Phase 3\n");
    
    return test_finalize();
}