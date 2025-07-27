#ifndef FLEXON_UTILS_H
#define FLEXON_UTILS_H

/* ============================================================================
 * FlexonDB Utility Functions
 * ============================================================================
 * This file contains utility functions used throughout FlexonDB.
 */

#include "config.h"
#include "types.h"
#include "error.h"
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* ============================================================================
 * File Utilities
 * ============================================================================ */

/**
 * Get file extension from path
 */
const char* utils_get_file_extension(const char* path);

/**
 * Get filename from full path (without directory)
 */
const char* utils_get_filename(const char* path);

/**
 * Join path components safely
 */
void utils_join_path(const char* dir, const char* filename, char* result, size_t result_size);

/**
 * Check if file exists
 */
bool utils_file_exists(const char* path);

/**
 * Get file size in bytes
 */
uint64_t utils_get_file_size(const char* path);

/**
 * Get file modification time
 */
time_t utils_get_file_mtime(const char* path);

/* ============================================================================
 * String Utilities
 * ============================================================================ */

/**
 * Format file size in human-readable format (B, KB, MB, GB)
 */
void utils_format_file_size(uint64_t bytes, char* buffer, size_t buffer_size);

/**
 * Format timestamp in human-readable format
 */
void utils_format_timestamp(time_t timestamp, char* buffer, size_t buffer_size);

/**
 * Trim whitespace from string
 */
char* utils_trim_string(char* str);

/**
 * Convert string to lowercase
 */
void utils_string_to_lower(char* str);

/**
 * Check if string is empty or only whitespace
 */
bool utils_string_is_empty(const char* str);

/**
 * Safe string copy with null termination
 */
void utils_safe_strcpy(char* dest, const char* src, size_t dest_size);

/**
 * Validate field name (alphanumeric + underscore, starts with letter)
 */
bool utils_validate_field_name(const char* name);

/**
 * Validate database name
 */
bool utils_validate_database_name(const char* name);

/* ============================================================================
 * Memory Utilities
 * ============================================================================ */

/**
 * Safe malloc with error handling
 */
void* utils_malloc(size_t size, error_context_t* error);

/**
 * Safe calloc with error handling
 */
void* utils_calloc(size_t count, size_t size, error_context_t* error);

/**
 * Safe free (sets pointer to NULL)
 */
void utils_free(void** ptr);

/**
 * Copy string with allocation
 */
char* utils_strdup(const char* str, error_context_t* error);

/* ============================================================================
 * Time Utilities
 * ============================================================================ */

/**
 * Get current timestamp
 */
uint64_t utils_get_timestamp(void);

/**
 * Start timing measurement
 */
void utils_timing_start(timing_info_enhanced_t* timing);

/**
 * End timing measurement and return elapsed milliseconds
 */
double utils_timing_end(timing_info_enhanced_t* timing);

/* ============================================================================
 * Platform Utilities
 * ============================================================================ */

/**
 * Get current user name
 */
void utils_get_current_user(char* buffer, size_t buffer_size);

/**
 * Get current working directory
 */
void utils_get_current_directory(char* buffer, size_t buffer_size);

/**
 * Get environment variable with default value
 */
const char* utils_getenv(const char* name, const char* default_value);

/* ============================================================================
 * Data Conversion Utilities
 * ============================================================================ */

/**
 * Convert string to int32 with error checking
 */
bool utils_str_to_int32(const char* str, int32_t* result);

/**
 * Convert string to float with error checking
 */
bool utils_str_to_float(const char* str, float* result);

/**
 * Convert string to bool with error checking
 */
bool utils_str_to_bool(const char* str, bool* result);

/**
 * Convert field type to string
 */
const char* utils_field_type_to_string(field_type_t type);

/**
 * Convert string to field type
 */
field_type_t utils_string_to_field_type(const char* type_str);

/**
 * Get size of field type in bytes
 */
uint32_t utils_get_field_type_size(field_type_t type);

/* ============================================================================
 * Checksum Utilities
 * ============================================================================ */

/**
 * Calculate simple checksum for data integrity
 */
uint32_t utils_simple_checksum(const void* data, size_t length);

/**
 * Verify data integrity using checksum
 */
bool utils_verify_checksum(const void* data, size_t length, uint32_t expected_checksum);

#endif // FLEXON_UTILS_H