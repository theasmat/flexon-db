#ifndef FLEXON_TYPES_H
#define FLEXON_TYPES_H

/* ============================================================================
 * FlexonDB Type Definitions
 * ============================================================================
 * This file contains common type definitions used throughout FlexonDB.
 * It only includes types that don't have circular dependencies.
 */

#define _POSIX_C_SOURCE 199309L
#include "config.h"
#include <stdint.h>
#include <stdio.h>
#include <time.h>

/* ============================================================================
 * Basic Database Types
 * ============================================================================ */

/**
 * Database header structure - Enhanced version
 * Contains metadata about the database file
 */
typedef struct {
    char magic[8];                 // Magic number string
    uint32_t version;              // File format version
    uint32_t schema_size;          // Size of schema section in bytes
    uint32_t data_size;            // Size of data section in bytes  
    uint32_t chunk_size;           // Number of rows per chunk
    uint64_t created_timestamp;    // File creation timestamp
    uint64_t modified_timestamp;   // Last modification timestamp
    uint32_t checksum;             // Header checksum for integrity
} db_header_t;

/**
 * Enhanced field definition structure
 * Describes a field with additional metadata
 */
typedef struct {
    char name[MAX_FIELD_NAME_LENGTH];  // Field name
    field_type_t type;                 // Field data type
    uint32_t size;                     // Size in bytes
    uint8_t nullable;                  // 1 if field can be NULL, 0 otherwise
    uint8_t indexed;                   // 1 if field is indexed, 0 otherwise
    uint8_t reserved[2];               // Reserved for future use
} field_def_enhanced_t;

/* ============================================================================
 * Configuration and Setup Types
 * ============================================================================ */

/**
 * Runtime configuration structure
 * Contains settings that can be changed at runtime
 */
typedef struct {
    char working_directory[MAX_PATH_LENGTH];
    char log_file[MAX_PATH_LENGTH];
    uint32_t default_chunk_size;
    uint32_t max_memory_usage;
    uint8_t enable_logging;
    uint8_t enable_debug;
    uint8_t enable_color_output;
    uint8_t enable_unicode_tables;
} runtime_config_t;

/**
 * File information structure
 * Contains metadata about database files
 */
typedef struct {
    char filename[MAX_PATH_LENGTH];    // Full file path
    char basename[256];                // Base filename without path
    uint64_t size;                     // File size in bytes
    time_t created;                    // Creation time
    time_t modified;                   // Last modification time
    uint32_t row_count;                // Number of rows (if known)
    uint32_t field_count;              // Number of fields (if known)
} file_info_t;

/**
 * Timing information structure
 * Used for measuring operation performance
 */
typedef struct {
    uint64_t start_time;           // Start time in nanoseconds
    uint64_t end_time;             // End time in nanoseconds
    double elapsed_ms;             // Elapsed time in milliseconds
} timing_info_enhanced_t;

/* ============================================================================
 * Error Handling Types
 * ============================================================================ */

/**
 * Error categories for different types of errors
 */
typedef enum {
    ERROR_NONE = 0,
    ERROR_FILE_IO,
    ERROR_MEMORY,
    ERROR_INVALID_SCHEMA,
    ERROR_INVALID_DATA,
    ERROR_DATABASE_CORRUPT,
    ERROR_PERMISSION,
    ERROR_DISK_SPACE,
    ERROR_NETWORK,
    ERROR_TIMEOUT,
    ERROR_UNKNOWN
} error_category_t;

/**
 * Error context structure
 * Contains detailed information about an error
 */
typedef struct {
    error_category_t category;     // Error category
    int code;                      // Specific error code
    char message[256];             // Human-readable error message
    char file[64];                 // Source file where error occurred
    int line;                      // Line number where error occurred
    char function[64];             // Function where error occurred
    char context[128];             // Additional context information
} error_context_t;

/* ============================================================================
 * Command and Shell Types
 * ============================================================================ */

/**
 * Command types for shell operations
 * Extended version of existing command types
 */
typedef enum {
    CMD_TYPE_UNKNOWN,
    CMD_TYPE_USE,
    CMD_TYPE_SHOW_DATABASES, 
    CMD_TYPE_CREATE,
    CMD_TYPE_DROP,
    CMD_TYPE_SELECT,
    CMD_TYPE_INSERT,
    CMD_TYPE_COUNT,
    CMD_TYPE_INFO,
    CMD_TYPE_SCHEMA,
    CMD_TYPE_STATUS,
    CMD_TYPE_HELP,
    CMD_TYPE_HISTORY,
    CMD_TYPE_CLEAR,
    CMD_TYPE_EXIT,
    CMD_TYPE_QUIT
} shell_command_type_t;

/**
 * Enhanced parsed command structure
 * Contains detailed information about a parsed command
 */
typedef struct {
    shell_command_type_t type;     // Command type
    char* args[16];                // Command arguments
    int arg_count;                 // Number of arguments
    char* raw_line;                // Original command line
    char* database_name;           // Target database (if applicable)
    char* schema_str;              // Schema string (if applicable)
    char* data_str;                // Data string (if applicable)
    error_context_t error;         // Error information
} parsed_command_enhanced_t;

/* ============================================================================
 * Performance and Statistics Types  
 * ============================================================================ */

/**
 * Database statistics structure
 * Contains performance and usage statistics
 */
typedef struct {
    uint64_t total_reads;          // Total read operations
    uint64_t total_writes;         // Total write operations
    uint64_t cache_hits;           // Cache hit count
    uint64_t cache_misses;         // Cache miss count
    double avg_read_time;          // Average read time in ms
    double avg_write_time;         // Average write time in ms
    uint64_t memory_usage;         // Current memory usage in bytes
    uint64_t peak_memory_usage;    // Peak memory usage in bytes
} db_statistics_t;

/**
 * Operation result structure
 * Generic result structure for database operations
 */
typedef struct {
    status_code_t status;          // Operation status
    uint32_t rows_affected;        // Number of rows affected
    double execution_time;         // Execution time in milliseconds
    char message[256];             // Result message
    error_context_t error;         // Error details (if any)
} operation_result_t;

#endif // FLEXON_TYPES_H