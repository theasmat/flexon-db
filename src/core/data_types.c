#include "../../include/core/data_types.h"
#include <string.h>
#include <stdlib.h>

// Type size lookup
size_t flexon_type_size(flexon_data_type_t type) {
    switch(type) {
        // String types
        case FLEXON_STRING16:   return 16;
        case FLEXON_STRING32:   return 32;
        case FLEXON_STRING64:   return 64;
        case FLEXON_STRING128:  return 128;
        case FLEXON_STRING256:  return 256;
        case FLEXON_STRING512:  return 512;
        case FLEXON_TEXT:       return 1024;  // Default for variable text
        
        // Integer types
        case FLEXON_INT8:       return 1;
        case FLEXON_INT16:      return 2;
        case FLEXON_INT32:      return 4;
        case FLEXON_INT64:      return 8;
        case FLEXON_UINT8:      return 1;
        case FLEXON_UINT16:     return 2;
        case FLEXON_UINT32:     return 4;
        case FLEXON_UINT64:     return 8;
        
        // Floating point types
        case FLEXON_FLOAT32:    return 4;
        case FLEXON_FLOAT64:    return 8;
        case FLEXON_DECIMAL:    return 16;  // Long double
        
        // Special types
        case FLEXON_BOOL:       return 1;
        case FLEXON_TIMESTAMP:  return 8;
        case FLEXON_DATE:       return 4;
        case FLEXON_UUID:       return 36;  // Standard UUID string length
        case FLEXON_JSON:       return 1024; // Default JSON object size
        case FLEXON_BLOB:       return 1024; // Default blob size
        
        default:                return 0;
    }
}

// Type name for display
const char* flexon_type_name(flexon_data_type_t type) {
    switch(type) {
        // String types
        case FLEXON_STRING16:   return "string16";
        case FLEXON_STRING32:   return "string32";
        case FLEXON_STRING64:   return "string64";
        case FLEXON_STRING128:  return "string128";
        case FLEXON_STRING256:  return "string256";
        case FLEXON_STRING512:  return "string512";
        case FLEXON_TEXT:       return "text";
        
        // Integer types
        case FLEXON_INT8:       return "int8";
        case FLEXON_INT16:      return "int16";
        case FLEXON_INT32:      return "int32";
        case FLEXON_INT64:      return "int64";
        case FLEXON_UINT8:      return "uint8";
        case FLEXON_UINT16:     return "uint16";
        case FLEXON_UINT32:     return "uint32";
        case FLEXON_UINT64:     return "uint64";
        
        // Floating point types
        case FLEXON_FLOAT32:    return "float32";
        case FLEXON_FLOAT64:    return "float64";
        case FLEXON_DECIMAL:    return "decimal";
        
        // Special types
        case FLEXON_BOOL:       return "bool";
        case FLEXON_TIMESTAMP:  return "timestamp";
        case FLEXON_DATE:       return "date";
        case FLEXON_UUID:       return "uuid";
        case FLEXON_JSON:       return "json";
        case FLEXON_BLOB:       return "blob";
        
        default:                return "unknown";
    }
}

// Parse type from string with smart defaults
flexon_data_type_t flexon_parse_type(const char* type_str) {
    if (!type_str) return FLEXON_TYPE_UNKNOWN;
    
    // String types with specific sizes
    if (strcmp(type_str, "string") == 0) return FLEXON_STRING256;  // Default string
    if (strcmp(type_str, "string16") == 0) return FLEXON_STRING16;
    if (strcmp(type_str, "string32") == 0) return FLEXON_STRING32;
    if (strcmp(type_str, "string64") == 0) return FLEXON_STRING64;
    if (strcmp(type_str, "string128") == 0) return FLEXON_STRING128;
    if (strcmp(type_str, "string256") == 0) return FLEXON_STRING256;
    if (strcmp(type_str, "string512") == 0) return FLEXON_STRING512;
    if (strcmp(type_str, "text") == 0) return FLEXON_TEXT;
    
    // Integer types
    if (strcmp(type_str, "int") == 0) return FLEXON_INT32;  // Default int
    if (strcmp(type_str, "int8") == 0) return FLEXON_INT8;
    if (strcmp(type_str, "int16") == 0) return FLEXON_INT16;
    if (strcmp(type_str, "int32") == 0) return FLEXON_INT32;
    if (strcmp(type_str, "int64") == 0) return FLEXON_INT64;
    if (strcmp(type_str, "uint8") == 0) return FLEXON_UINT8;
    if (strcmp(type_str, "uint16") == 0) return FLEXON_UINT16;
    if (strcmp(type_str, "uint32") == 0) return FLEXON_UINT32;
    if (strcmp(type_str, "uint64") == 0) return FLEXON_UINT64;
    
    // Floating point types
    if (strcmp(type_str, "float") == 0) return FLEXON_FLOAT32;  // Default float
    if (strcmp(type_str, "float32") == 0) return FLEXON_FLOAT32;
    if (strcmp(type_str, "float64") == 0) return FLEXON_FLOAT64;
    if (strcmp(type_str, "double") == 0) return FLEXON_FLOAT64;
    if (strcmp(type_str, "decimal") == 0) return FLEXON_DECIMAL;
    if (strcmp(type_str, "num") == 0) return FLEXON_FLOAT32;    // Alias
    if (strcmp(type_str, "bignum") == 0) return FLEXON_FLOAT64; // Alias
    
    // Special types
    if (strcmp(type_str, "bool") == 0) return FLEXON_BOOL;
    if (strcmp(type_str, "timestamp") == 0) return FLEXON_TIMESTAMP;
    if (strcmp(type_str, "date") == 0) return FLEXON_DATE;
    if (strcmp(type_str, "uuid") == 0) return FLEXON_UUID;
    if (strcmp(type_str, "json") == 0) return FLEXON_JSON;
    if (strcmp(type_str, "blob") == 0) return FLEXON_BLOB;
    
    return FLEXON_TYPE_UNKNOWN; // Default fallback
}

// Check if type is a string type
int flexon_is_string_type(flexon_data_type_t type) {
    return (type >= FLEXON_STRING16 && type <= FLEXON_TEXT);
}

// Check if type is an integer type
int flexon_is_integer_type(flexon_data_type_t type) {
    return (type >= FLEXON_INT8 && type <= FLEXON_UINT64);
}

// Check if type is a floating point type
int flexon_is_float_type(flexon_data_type_t type) {
    return (type >= FLEXON_FLOAT32 && type <= FLEXON_DECIMAL);
}

// Get the string length for string types
size_t flexon_string_type_length(flexon_data_type_t type) {
    if (!flexon_is_string_type(type)) return 0;
    
    switch(type) {
        case FLEXON_STRING16:   return 16;
        case FLEXON_STRING32:   return 32;
        case FLEXON_STRING64:   return 64;
        case FLEXON_STRING128:  return 128;
        case FLEXON_STRING256:  return 256;
        case FLEXON_STRING512:  return 512;
        case FLEXON_TEXT:       return 1024;  // Variable, this is max
        default:                return 256;   // Default string length
    }
}