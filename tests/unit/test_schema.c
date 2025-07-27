#include "../test_utils.h"
#include "../../include/schema.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    test_init("Enhanced Schema Module Tests");
    
    // Test 1: Basic schema parsing (existing functionality)
    printf("Test 1: Basic schema parsing\n");
    schema_t* schema = parse_schema("name string, age int32, salary float, active bool");
    test_assert_not_null(schema, "Schema should be parsed successfully");
    test_assert_equal_int(4, schema->field_count, "Schema should have 4 fields");
    test_assert_equal_int(265, schema->row_size, "Row size should be 265 bytes");
    
    if (schema) {
        test_assert_equal_str("name", schema->fields[0].name, "First field name");
        test_assert_equal_str("age", schema->fields[1].name, "Second field name");
        test_assert_equal_str("salary", schema->fields[2].name, "Third field name");
        test_assert_equal_str("active", schema->fields[3].name, "Fourth field name");
        
        test_assert(schema->fields[0].type == FIELD_TYPE_STRING, "First field type is string");
        test_assert(schema->fields[1].type == FIELD_TYPE_INT32, "Second field type is int32");
        test_assert(schema->fields[2].type == FIELD_TYPE_FLOAT, "Third field type is float");
        test_assert(schema->fields[3].type == FIELD_TYPE_BOOL, "Fourth field type is bool");
        
        free_schema(schema);
    }
    
    // Test 2: Field index lookup
    printf("\nTest 2: Field index lookup\n");
    schema = parse_schema("id int32, name string, email string");
    test_assert_not_null(schema, "Schema should be parsed successfully");
    
    if (schema) {
        test_assert_equal_int(0, get_field_index(schema, "id"), "ID field index");
        test_assert_equal_int(1, get_field_index(schema, "name"), "Name field index");
        test_assert_equal_int(2, get_field_index(schema, "email"), "Email field index");
        test_assert_equal_int(-1, get_field_index(schema, "nonexistent"), "Non-existent field index");
        
        free_schema(schema);
    }
    
    // Test 3: Schema validation
    printf("\nTest 3: Schema validation\n");
    schema_t* invalid_schema = parse_schema("name string, name int32");  // Duplicate field
    test_assert(invalid_schema == NULL, "Schema with duplicate fields should be rejected");
    
    schema_t* empty_schema = parse_schema("");
    test_assert(empty_schema == NULL, "Empty schema should be rejected");
    
    schema_t* malformed_schema = parse_schema("name, age int32");
    test_assert(malformed_schema == NULL, "Malformed schema should be rejected");
    
    // Test 4: Type string conversions
    printf("\nTest 4: Type string conversions\n");
    test_assert_equal_str("int32", field_type_to_string(FIELD_TYPE_INT32), "int32 type name");
    test_assert_equal_str("float", field_type_to_string(FIELD_TYPE_FLOAT), "float type name");
    test_assert_equal_str("string", field_type_to_string(FIELD_TYPE_STRING), "string type name");
    test_assert_equal_str("bool", field_type_to_string(FIELD_TYPE_BOOL), "bool type name");
    test_assert_equal_str("unknown", field_type_to_string(FIELD_TYPE_UNKNOWN), "unknown type name");
    
    test_assert(string_to_field_type("int32") == FIELD_TYPE_INT32, "Parse int32 type");
    test_assert(string_to_field_type("float") == FIELD_TYPE_FLOAT, "Parse float type");
    test_assert(string_to_field_type("string") == FIELD_TYPE_STRING, "Parse string type");
    test_assert(string_to_field_type("bool") == FIELD_TYPE_BOOL, "Parse bool type");
    test_assert(string_to_field_type("invalid") == FIELD_TYPE_UNKNOWN, "Parse invalid type");
    
    // Test 5: Large schema handling
    printf("\nTest 5: Large schema handling\n");
    char large_schema_str[2048] = "";
    for (int i = 0; i < 50; i++) {
        char field[64];
        snprintf(field, sizeof(field), "field%d int32%s", i, (i < 49) ? ", " : "");
        strcat(large_schema_str, field);
    }
    
    schema_t* large_schema = parse_schema(large_schema_str);
    test_assert_not_null(large_schema, "Large schema should be parsed");
    if (large_schema) {
        test_assert_equal_int(50, large_schema->field_count, "Large schema field count");
        free_schema(large_schema);
    }
    
    // Test 6: Row size calculation
    printf("\nTest 6: Row size calculation\n");
    schema_t* size_test_schema = parse_schema("tiny_int int32, big_string string, small_bool bool, decimal_val float");
    test_assert_not_null(size_test_schema, "Size test schema should be parsed");
    if (size_test_schema) {
        uint32_t expected_size = 4 + 256 + 1 + 4;  // int32 + string + bool + float
        test_assert_equal_int(expected_size, calculate_row_size(size_test_schema), "Row size calculation");
        test_assert_equal_int(expected_size, size_test_schema->row_size, "Schema row size matches calculation");
        free_schema(size_test_schema);
    }
    
    printf("\n");
    return test_finalize();
}