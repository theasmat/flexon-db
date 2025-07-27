#include "../test_utils.h"
#include "../../include/config.h"
#include "../../include/core/data_types.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    test_init("Enhanced Data Types System Tests");
    
    // Test 1: Legacy data type compatibility
    printf("Test 1: Legacy data types compatibility\n");
    test_assert(TYPE_INT32 == 0, "INT32 type value");
    test_assert(TYPE_FLOAT == 1, "FLOAT type value");
    test_assert(TYPE_STRING == 2, "STRING type value");
    test_assert(TYPE_BOOL == 3, "BOOL type value");
    test_assert(TYPE_UNKNOWN == 4, "UNKNOWN type value");
    
    // Test 2: Extended data types parsing
    printf("\nTest 2: Extended data types parsing\n");
    test_assert(flexon_parse_type("string") == FLEXON_STRING256, "Default string maps to string256");
    test_assert(flexon_parse_type("string16") == FLEXON_STRING16, "string16 type");
    test_assert(flexon_parse_type("string32") == FLEXON_STRING32, "string32 type");
    test_assert(flexon_parse_type("string64") == FLEXON_STRING64, "string64 type");
    test_assert(flexon_parse_type("string128") == FLEXON_STRING128, "string128 type");
    test_assert(flexon_parse_type("string256") == FLEXON_STRING256, "string256 type");
    test_assert(flexon_parse_type("string512") == FLEXON_STRING512, "string512 type");
    test_assert(flexon_parse_type("text") == FLEXON_TEXT, "text type");
    
    // Test 3: Integer types
    printf("\nTest 3: Integer type parsing\n");
    test_assert(flexon_parse_type("int") == FLEXON_INT32, "Default int maps to int32");
    test_assert(flexon_parse_type("int8") == FLEXON_INT8, "int8 type");
    test_assert(flexon_parse_type("int16") == FLEXON_INT16, "int16 type");
    test_assert(flexon_parse_type("int32") == FLEXON_INT32, "int32 type");
    test_assert(flexon_parse_type("int64") == FLEXON_INT64, "int64 type");
    test_assert(flexon_parse_type("uint8") == FLEXON_UINT8, "uint8 type");
    test_assert(flexon_parse_type("uint16") == FLEXON_UINT16, "uint16 type");
    test_assert(flexon_parse_type("uint32") == FLEXON_UINT32, "uint32 type");
    test_assert(flexon_parse_type("uint64") == FLEXON_UINT64, "uint64 type");
    
    // Test 4: Floating point types
    printf("\nTest 4: Floating point type parsing\n");
    test_assert(flexon_parse_type("float") == FLEXON_FLOAT32, "Default float maps to float32");
    test_assert(flexon_parse_type("float32") == FLEXON_FLOAT32, "float32 type");
    test_assert(flexon_parse_type("float64") == FLEXON_FLOAT64, "float64 type");
    test_assert(flexon_parse_type("double") == FLEXON_FLOAT64, "double alias");
    test_assert(flexon_parse_type("decimal") == FLEXON_DECIMAL, "decimal type");
    test_assert(flexon_parse_type("num") == FLEXON_FLOAT32, "num alias");
    test_assert(flexon_parse_type("bignum") == FLEXON_FLOAT64, "bignum alias");
    
    // Test 5: Special types
    printf("\nTest 5: Special type parsing\n");
    test_assert(flexon_parse_type("bool") == FLEXON_BOOL, "bool type");
    test_assert(flexon_parse_type("timestamp") == FLEXON_TIMESTAMP, "timestamp type");
    test_assert(flexon_parse_type("date") == FLEXON_DATE, "date type");
    test_assert(flexon_parse_type("uuid") == FLEXON_UUID, "uuid type");
    test_assert(flexon_parse_type("json") == FLEXON_JSON, "json type");
    test_assert(flexon_parse_type("blob") == FLEXON_BLOB, "blob type");
    
    // Test 6: Type size calculations
    printf("\nTest 6: Type size calculations\n");
    test_assert_equal_int(16, flexon_type_size(FLEXON_STRING16), "string16 size");
    test_assert_equal_int(32, flexon_type_size(FLEXON_STRING32), "string32 size");
    test_assert_equal_int(64, flexon_type_size(FLEXON_STRING64), "string64 size");
    test_assert_equal_int(256, flexon_type_size(FLEXON_STRING256), "string256 size");
    test_assert_equal_int(1, flexon_type_size(FLEXON_INT8), "int8 size");
    test_assert_equal_int(2, flexon_type_size(FLEXON_INT16), "int16 size");
    test_assert_equal_int(4, flexon_type_size(FLEXON_INT32), "int32 size");
    test_assert_equal_int(8, flexon_type_size(FLEXON_INT64), "int64 size");
    test_assert_equal_int(4, flexon_type_size(FLEXON_FLOAT32), "float32 size");
    test_assert_equal_int(8, flexon_type_size(FLEXON_FLOAT64), "float64 size");
    
    // Test 7: Type name display
    printf("\nTest 7: Type name display\n");
    test_assert_equal_str("string16", flexon_type_name(FLEXON_STRING16), "string16 name");
    test_assert_equal_str("int32", flexon_type_name(FLEXON_INT32), "int32 name");
    test_assert_equal_str("float64", flexon_type_name(FLEXON_FLOAT64), "float64 name");
    test_assert_equal_str("bool", flexon_type_name(FLEXON_BOOL), "bool name");
    test_assert_equal_str("timestamp", flexon_type_name(FLEXON_TIMESTAMP), "timestamp name");
    
    // Test 8: Type classification functions
    printf("\nTest 8: Type classification\n");
    test_assert(flexon_is_string_type(FLEXON_STRING32), "string32 is string type");
    test_assert(flexon_is_string_type(FLEXON_TEXT), "text is string type");
    test_assert(!flexon_is_string_type(FLEXON_INT32), "int32 is not string type");
    
    test_assert(flexon_is_integer_type(FLEXON_INT32), "int32 is integer type");
    test_assert(flexon_is_integer_type(FLEXON_UINT64), "uint64 is integer type");
    test_assert(!flexon_is_integer_type(FLEXON_FLOAT32), "float32 is not integer type");
    
    test_assert(flexon_is_float_type(FLEXON_FLOAT32), "float32 is float type");
    test_assert(flexon_is_float_type(FLEXON_FLOAT64), "float64 is float type");
    test_assert(!flexon_is_float_type(FLEXON_INT32), "int32 is not float type");
    
    // Test 9: Backward compatibility mapping
    printf("\nTest 9: Backward compatibility\n");
    test_assert(flexon_to_legacy_type(FLEXON_INT32) == TYPE_INT32, "int32 maps to legacy INT32");
    test_assert(flexon_to_legacy_type(FLEXON_INT64) == TYPE_INT32, "int64 maps to legacy INT32");
    test_assert(flexon_to_legacy_type(FLEXON_FLOAT32) == TYPE_FLOAT, "float32 maps to legacy FLOAT");
    test_assert(flexon_to_legacy_type(FLEXON_STRING256) == TYPE_STRING, "string256 maps to legacy STRING");
    test_assert(flexon_to_legacy_type(FLEXON_BOOL) == TYPE_BOOL, "bool maps to legacy BOOL");
    
    test_assert(legacy_to_flexon_type(TYPE_INT32) == FLEXON_INT32, "legacy INT32 maps to int32");
    test_assert(legacy_to_flexon_type(TYPE_FLOAT) == FLEXON_FLOAT32, "legacy FLOAT maps to float32");
    test_assert(legacy_to_flexon_type(TYPE_STRING) == FLEXON_STRING256, "legacy STRING maps to string256");
    test_assert(legacy_to_flexon_type(TYPE_BOOL) == FLEXON_BOOL, "legacy BOOL maps to bool");
    
    printf("\n");
    return test_finalize();
}