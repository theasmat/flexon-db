#ifndef CORE_DATA_TYPES_H
#define CORE_DATA_TYPES_H

#include <stddef.h>
#include <stdint.h>

/**
 * FlexonDB Extended Data Type System
 * 
 * This file defines the comprehensive data type system for FlexonDB with 20+ types,
 * smart defaults, and backward compatibility with the original 4 types.
 */

typedef enum {
    // String types with specific sizes
    FLEXON_STRING16 = 0x10,    // 16 character string
    FLEXON_STRING32 = 0x11,    // 32 character string  
    FLEXON_STRING64 = 0x12,    // 64 character string
    FLEXON_STRING128 = 0x13,   // 128 character string
    FLEXON_STRING256 = 0x14,   // 256 character string (default)
    FLEXON_STRING512 = 0x15,   // 512 character string
    FLEXON_TEXT = 0x16,        // Variable length text
    
    // Integer types
    FLEXON_INT8 = 0x20,        // 8-bit signed integer
    FLEXON_INT16 = 0x21,       // 16-bit signed integer  
    FLEXON_INT32 = 0x22,       // 32-bit signed integer (default)
    FLEXON_INT64 = 0x23,       // 64-bit signed integer
    FLEXON_UINT8 = 0x24,       // 8-bit unsigned integer
    FLEXON_UINT16 = 0x25,      // 16-bit unsigned integer
    FLEXON_UINT32 = 0x26,      // 32-bit unsigned integer
    FLEXON_UINT64 = 0x27,      // 64-bit unsigned integer
    
    // Floating point types
    FLEXON_FLOAT32 = 0x30,     // 32-bit float (default)
    FLEXON_FLOAT64 = 0x31,     // 64-bit double
    FLEXON_DECIMAL = 0x32,     // Long double precision
    
    // Special types
    FLEXON_BOOL = 0x40,        // Boolean
    FLEXON_TIMESTAMP = 0x41,   // Unix timestamp
    FLEXON_DATE = 0x42,        // Date only
    FLEXON_UUID = 0x43,        // UUID string
    FLEXON_JSON = 0x44,        // JSON object
    FLEXON_BLOB = 0x45,        // Binary data
    
    // Aliases for convenience and backward compatibility
    FLEXON_STRING = FLEXON_STRING256,  // Default string
    FLEXON_INT = FLEXON_INT32,         // Default integer
    FLEXON_FLOAT = FLEXON_FLOAT32,     // Default float
    FLEXON_NUM = FLEXON_FLOAT32,       // Alias for float
    FLEXON_DOUBLE = FLEXON_FLOAT64,    // Alias for double
    FLEXON_BIGNUM = FLEXON_FLOAT64,    // Alias for double
    
    // Backward compatibility with original types
    FLEXON_TYPE_INT32 = FLEXON_INT32,
    FLEXON_TYPE_FLOAT = FLEXON_FLOAT32,
    FLEXON_TYPE_STRING = FLEXON_STRING256,
    FLEXON_TYPE_BOOL = FLEXON_BOOL,
    FLEXON_TYPE_UNKNOWN = 0xFF
} flexon_data_type_t;

/**
 * Type size lookup
 * Returns the size in bytes for a given data type
 */
size_t flexon_type_size(flexon_data_type_t type);

/**
 * Type name for display
 * Returns a human-readable string representation of the type
 */
const char* flexon_type_name(flexon_data_type_t type);

/**
 * Parse type from string with smart defaults
 * Handles all type names including aliases and provides intelligent fallbacks
 */
flexon_data_type_t flexon_parse_type(const char* type_str);

/**
 * Check if type is a string type
 */
int flexon_is_string_type(flexon_data_type_t type);

/**
 * Check if type is an integer type
 */
int flexon_is_integer_type(flexon_data_type_t type);

/**
 * Check if type is a floating point type
 */
int flexon_is_float_type(flexon_data_type_t type);

/**
 * Get the string length for string types
 */
size_t flexon_string_type_length(flexon_data_type_t type);

/**
 * Map new types to old field_type_t for backward compatibility
 */
#include "../config.h"

static inline field_type_t flexon_to_legacy_type(flexon_data_type_t type) {
    switch(type) {
        case FLEXON_INT8:
        case FLEXON_INT16:
        case FLEXON_INT32:
        case FLEXON_INT64:
        case FLEXON_UINT8:
        case FLEXON_UINT16:
        case FLEXON_UINT32:
        case FLEXON_UINT64:
            return TYPE_INT32;
        case FLEXON_FLOAT32:
        case FLEXON_FLOAT64:
        case FLEXON_DECIMAL:
            return TYPE_FLOAT;
        case FLEXON_STRING16:
        case FLEXON_STRING32:
        case FLEXON_STRING64:
        case FLEXON_STRING128:
        case FLEXON_STRING256:
        case FLEXON_STRING512:
        case FLEXON_TEXT:
            return TYPE_STRING;
        case FLEXON_BOOL:
            return TYPE_BOOL;
        default:
            return TYPE_UNKNOWN;
    }
}

static inline flexon_data_type_t legacy_to_flexon_type(field_type_t type) {
    switch(type) {
        case TYPE_INT32:
            return FLEXON_INT32;
        case TYPE_FLOAT:
            return FLEXON_FLOAT32;
        case TYPE_STRING:
            return FLEXON_STRING256;
        case TYPE_BOOL:
            return FLEXON_BOOL;
        default:
            return FLEXON_TYPE_UNKNOWN;
    }
}

#endif // CORE_DATA_TYPES_H