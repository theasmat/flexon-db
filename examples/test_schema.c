#include "../include/schema.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("=== FlexonDB Schema Module Test ===\n\n");
    
    // Test 1: Parse a simple schema
    printf("Test 1: Parsing schema 'name string, age int32, salary float, active bool'\n");
    schema_t* schema = parse_schema("name string, age int32, salary float, active bool");
    
    if (schema) {
        print_schema(schema);
        printf("\n");
        
        // Test field lookup
        int name_idx = get_field_index(schema, "name");
        int age_idx = get_field_index(schema, "age");
        int invalid_idx = get_field_index(schema, "invalid");
        
        printf("Field indexes:\n");
        printf("  'name' -> %d\n", name_idx);
        printf("  'age' -> %d\n", age_idx);
        printf("  'invalid' -> %d\n", invalid_idx);
        
        free_schema(schema);
    } else {
        printf("Failed to parse schema!\n");
    }
    
    printf("\n");
    
    // Test 2: Invalid schema
    printf("Test 2: Parsing invalid schema 'name, age int32'\n");
    schema_t* invalid_schema = parse_schema("name, age int32");
    if (!invalid_schema) {
        printf("Correctly rejected invalid schema\n");
    }
    
    printf("\n");
    
    // Test 3: Duplicate fields
    printf("Test 3: Parsing schema with duplicate fields 'name string, name int32'\n");
    schema_t* dup_schema = parse_schema("name string, name int32");
    if (!dup_schema) {
        printf("Correctly rejected schema with duplicate fields\n");
    }
    
    printf("\n");
    
    // Test 4: Field type conversion
    printf("Test 4: Field type conversions\n");
    printf("  'int32' -> %s\n", field_type_to_string(string_to_field_type("int32")));
    printf("  'float' -> %s\n", field_type_to_string(string_to_field_type("float")));
    printf("  'string' -> %s\n", field_type_to_string(string_to_field_type("string")));
    printf("  'bool' -> %s\n", field_type_to_string(string_to_field_type("bool")));
    printf("  'invalid' -> %s\n", field_type_to_string(string_to_field_type("invalid")));
    
    printf("\n=== Test Complete ===\n");
    return 0;
}