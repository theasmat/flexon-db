#include "../test_utils.h" 
#include "../../include/schema.h"
#include "../../include/core/data_types.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    test_init("Extended Types Schema Integration Tests");
    
    // Test 1: Extended types can be parsed and mapped to legacy types
    printf("Test 1: Extended type parsing with legacy mapping\n");
    
    // Test various extended types that should map to legacy types
    schema_t* schema1 = parse_schema("id int64, name string32, score float64, active bool");
    test_assert_not_null(schema1, "Schema with extended types should be parsed");
    if (schema1) {
        test_assert_equal_int(4, schema1->field_count, "Should have 4 fields");
        
        // The types should be mapped to legacy types but the sizes should be enhanced
        test_assert(schema1->fields[0].type == FIELD_TYPE_INT32, "int64 maps to legacy int32");
        test_assert(schema1->fields[1].type == FIELD_TYPE_STRING, "string32 maps to legacy string");  
        test_assert(schema1->fields[2].type == FIELD_TYPE_FLOAT, "float64 maps to legacy float");
        test_assert(schema1->fields[3].type == FIELD_TYPE_BOOL, "bool maps to legacy bool");
        
        // The sizes should reflect the enhanced types
        test_assert_equal_int(8, schema1->fields[0].size, "int64 should have 8 byte size");
        test_assert_equal_int(32, schema1->fields[1].size, "string32 should have 32 byte size");
        test_assert_equal_int(8, schema1->fields[2].size, "float64 should have 8 byte size");
        test_assert_equal_int(1, schema1->fields[3].size, "bool should have 1 byte size");
        
        free_schema(schema1);
    }
    
    // Test 2: Various string sizes
    printf("\nTest 2: Various string sizes\n");
    schema_t* schema2 = parse_schema("tiny string16, small string32, medium string64, large string128, huge string256, giant string512");
    test_assert_not_null(schema2, "Schema with various string sizes should be parsed");
    if (schema2) {
        test_assert_equal_int(6, schema2->field_count, "Should have 6 fields");
        test_assert_equal_int(16, schema2->fields[0].size, "string16 size");
        test_assert_equal_int(32, schema2->fields[1].size, "string32 size");
        test_assert_equal_int(64, schema2->fields[2].size, "string64 size");
        test_assert_equal_int(128, schema2->fields[3].size, "string128 size");
        test_assert_equal_int(256, schema2->fields[4].size, "string256 size");
        test_assert_equal_int(512, schema2->fields[5].size, "string512 size");
        
        free_schema(schema2);
    }
    
    // Test 3: Smart defaults
    printf("\nTest 3: Smart defaults parsing\n");
    schema_t* schema3 = parse_schema("id int, name string, amount float, flag bool");
    test_assert_not_null(schema3, "Schema with default types should be parsed");
    if (schema3) {
        test_assert_equal_int(4, schema3->field_count, "Should have 4 fields");
        // Default types should map to their enhanced equivalents with default sizes
        test_assert_equal_int(4, schema3->fields[0].size, "default int -> int32 (4 bytes)");
        test_assert_equal_int(256, schema3->fields[1].size, "default string -> string256 (256 bytes)");
        test_assert_equal_int(4, schema3->fields[2].size, "default float -> float32 (4 bytes)");
        test_assert_equal_int(1, schema3->fields[3].size, "default bool -> bool (1 byte)");
        
        free_schema(schema3);
    }
    
    // Test 4: Aliases should work
    printf("\nTest 4: Type aliases\n");
    schema_t* schema4 = parse_schema("value num, precision double, count bignum");
    test_assert_not_null(schema4, "Schema with type aliases should be parsed");
    if (schema4) {
        test_assert_equal_int(3, schema4->field_count, "Should have 3 fields");
        test_assert_equal_int(4, schema4->fields[0].size, "num -> float32 (4 bytes)");
        test_assert_equal_int(8, schema4->fields[1].size, "double -> float64 (8 bytes)");
        test_assert_equal_int(8, schema4->fields[2].size, "bignum -> float64 (8 bytes)");
        
        free_schema(schema4);
    }
    
    // Test 5: Row size calculation with mixed types
    printf("\nTest 5: Row size calculation with mixed enhanced types\n");
    schema_t* schema5 = parse_schema("id int16, code string64, value float64, flag bool");
    test_assert_not_null(schema5, "Mixed enhanced types schema should be parsed");
    if (schema5) {
        uint32_t expected_size = 2 + 64 + 8 + 1;  // int16 + string64 + float64 + bool
        test_assert_equal_int(expected_size, schema5->row_size, "Row size should be calculated from enhanced types");
        
        free_schema(schema5);
    }
    
    // Test 6: Verify that enhanced types are rejected when they should be
    printf("\nTest 6: Enhanced types that don't map to legacy types\n");
    // These types are not supported by the legacy system but should be parsed by enhanced parser
    flexon_data_type_t timestamp_type = flexon_parse_type("timestamp");
    flexon_data_type_t uuid_type = flexon_parse_type("uuid");
    flexon_data_type_t json_type = flexon_parse_type("json");
    
    test_assert(timestamp_type == FLEXON_TIMESTAMP, "timestamp type should be parsed");
    test_assert(uuid_type == FLEXON_UUID, "uuid type should be parsed");
    test_assert(json_type == FLEXON_JSON, "json type should be parsed");
    
    test_assert_equal_int(8, flexon_type_size(timestamp_type), "timestamp size");
    test_assert_equal_int(36, flexon_type_size(uuid_type), "uuid size");
    test_assert_equal_int(1024, flexon_type_size(json_type), "json size");
    
    printf("\n");
    return test_finalize();
}