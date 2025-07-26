#ifndef FLEXON_CONFIG_H
#define FLEXON_CONFIG_H

/* ============================================================================
 * FlexonDB Centralized Configuration
 * ============================================================================
 * This file contains centralized constants, limits, and compile-time switches.
 * It provides backward compatibility with existing code while centralizing
 * configuration values.
 */

#include <stdint.h>

/* ============================================================================
 * File Extensions
 * ============================================================================ */
#define FXDB_EXT ".fxdb"
#define TEMP_EXT ".tmp"
#define BACKUP_EXT ".bak"

/* ============================================================================
 * Database Limits
 * ============================================================================ */
#define MAX_COLUMNS 64
#define MAX_FIELD_NAME_LENGTH 64
#define MAX_STRING_LENGTH 256
#define MAX_CHUNK_SIZE 1000
#define MAX_DATABASES 256
#define MAX_COMMAND_LENGTH 1024
#define MAX_PATH_LENGTH 4096

/* ============================================================================
 * Performance and I/O Configuration
 * ============================================================================ */
#define FXDB_BUFFER_SIZE 4096      // Standard I/O buffer size (4KB)
#define FXDB_LARGE_BUFFER_SIZE 16384  // Large I/O buffer size (16KB)
#define FXDB_MIN_MMAP_SIZE 1024    // Minimum file size for memory mapping

/* ============================================================================
 * Common Strings and Magic Numbers
 * ============================================================================ */
#define DB_MAGIC "FXDB01"
#define DB_MAGIC_NUM 0x42445846        // "FXDB" as 32-bit integer
#define DB_VERSION 1                   // Current file format version
#define VERSION_STRING "1.0.0"
#define SHELL_PROMPT "flexondb> "
#define SHELL_PROMPT_WITH_DB "flexondb:%s> "
#define WELCOME_MESSAGE "FlexonDB Interactive Shell v" VERSION_STRING

/* ============================================================================
 * Compile-time Switches
 * ============================================================================ */
#define ENABLE_LOGGING 1
#define ENABLE_DEBUG 0
#define ENABLE_READLINE 1
#define ENABLE_COLOR_OUTPUT 1
#define ENABLE_UNICODE_TABLES 1

/* ============================================================================
 * Type System Enumerations
 * ============================================================================ */
typedef enum {
    TYPE_INT32,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_UNKNOWN
} field_type_t;

typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR = -1,
    STATUS_NOT_FOUND = -2,
    STATUS_INVALID_INPUT = -3,
    STATUS_PERMISSION_DENIED = -4,
    STATUS_DISK_FULL = -5
} status_code_t;

/* ============================================================================
 * Legacy Compatibility (for existing code)
 * ============================================================================ */
#define MAX_FIELD_NAME MAX_FIELD_NAME_LENGTH  // For backward compatibility
#define FXDB_MAGIC DB_MAGIC                   // For backward compatibility (string)
#define FXDB_VERSION 1                        // Keep existing version number

// Legacy field type names (map to new enum)
#define FIELD_INT TYPE_INT32
#define FIELD_FLOAT TYPE_FLOAT
#define FIELD_STRING TYPE_STRING
#define FIELD_BOOL TYPE_BOOL

#endif // FLEXON_CONFIG_H