#ifndef FLEXON_IO_UTILS_H
#define FLEXON_IO_UTILS_H

/* ============================================================================
 * FlexonDB I/O Utilities
 * ============================================================================
 * Centralized I/O helper functions for high-performance database operations.
 * Implements buffered I/O, memory mapping, and atomic file operations.
 */

#include "config.h"
#include "types.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>

/* ============================================================================
 * Buffered Writer Structure
 * ============================================================================ */

/**
 * Buffered writer for high-performance sequential writes
 * Uses large buffers to minimize syscalls and improve throughput
 */
typedef struct {
    FILE *file;                           // File handle
    char buffer[FXDB_BUFFER_SIZE];        // Write buffer
    size_t buffer_pos;                    // Current position in buffer
    size_t total_written;                 // Total bytes written
    int lock_fd;                          // File lock descriptor
    bool is_locked;                       // Lock status
} fxdb_buffered_writer_t;

/* ============================================================================
 * Memory-Mapped Reader Structure
 * ============================================================================ */

/**
 * Memory-mapped reader for high-performance random access reads
 * Uses mmap for zero-copy access to file data
 */
typedef struct {
    void *mmap_data;                      // Memory-mapped file data
    size_t file_size;                     // Size of mapped file
    int fd;                               // File descriptor
    bool is_mapped;                       // Mapping status
} fxdb_mmap_reader_t;

/* ============================================================================
 * Buffered Writing Functions
 * ============================================================================ */

/**
 * Create a new buffered writer for the specified file
 * @param filename Path to file to create/open for writing
 * @param lock_file Whether to lock the file exclusively
 * @return Buffered writer pointer on success, NULL on failure
 */
fxdb_buffered_writer_t* fxdb_buffered_writer_create(const char* filename, bool lock_file);

/**
 * Write uint32_t value to buffered writer
 * @param writer Buffered writer instance
 * @param value 32-bit unsigned integer to write
 * @return 0 on success, -1 on failure
 */
int fxdb_write_uint32(fxdb_buffered_writer_t* writer, uint32_t value);

/**
 * Write string to buffered writer with length prefix
 * @param writer Buffered writer instance
 * @param str String to write (null-terminated)
 * @param max_len Maximum length to write
 * @return 0 on success, -1 on failure
 */
int fxdb_write_string(fxdb_buffered_writer_t* writer, const char* str, size_t max_len);

/**
 * Write raw data to buffered writer
 * @param writer Buffered writer instance
 * @param data Pointer to data to write
 * @param size Number of bytes to write
 * @return 0 on success, -1 on failure
 */
int fxdb_write_data(fxdb_buffered_writer_t* writer, const void* data, size_t size);

/**
 * Flush buffered writer to disk
 * @param writer Buffered writer instance
 * @return 0 on success, -1 on failure
 */
int fxdb_flush_buffer(fxdb_buffered_writer_t* writer);

/**
 * Close buffered writer and release resources
 * @param writer Buffered writer instance
 * @return 0 on success, -1 on failure
 */
int fxdb_buffered_writer_close(fxdb_buffered_writer_t* writer);

/* ============================================================================
 * Memory-Mapped Reading Functions
 * ============================================================================ */

/**
 * Create a memory-mapped reader for the specified file
 * @param filename Path to file to open for reading
 * @return Memory-mapped reader on success, NULL on failure
 */
fxdb_mmap_reader_t* fxdb_mmap_reader_open(const char* filename);

/**
 * Read uint32_t value from memory-mapped file
 * @param reader Memory-mapped reader instance
 * @param offset Byte offset in file
 * @return uint32_t value at specified offset
 */
uint32_t fxdb_read_uint32_mmap(fxdb_mmap_reader_t* reader, size_t offset);

/**
 * Read string from memory-mapped file
 * @param reader Memory-mapped reader instance
 * @param offset Byte offset in file
 * @param out Output buffer for string
 * @param max_len Maximum length to read
 * @return Number of bytes read, or -1 on failure
 */
int fxdb_read_string_mmap(fxdb_mmap_reader_t* reader, size_t offset, char* out, size_t max_len);

/**
 * Get pointer to data at offset in memory-mapped file
 * @param reader Memory-mapped reader instance
 * @param offset Byte offset in file
 * @return Pointer to data at offset, or NULL if out of bounds
 */
void* fxdb_mmap_get_ptr(fxdb_mmap_reader_t* reader, size_t offset);

/**
 * Close memory-mapped reader and release resources
 * @param reader Memory-mapped reader instance
 */
void fxdb_mmap_reader_close(fxdb_mmap_reader_t* reader);

/* ============================================================================
 * File Management Functions
 * ============================================================================ */

/**
 * Normalize filename to ensure .fxdb extension
 * @param input Input filename (may or may not have extension)
 * @return Allocated string with normalized filename (caller must free)
 */
char* fxdb_normalize_filename(const char* input);

/**
 * Check if filename has .fxdb extension
 * @param filename Filename to check
 * @return true if has .fxdb extension, false otherwise
 */
bool fxdb_has_extension(const char* filename);

/**
 * Check if database file exists
 * @param filename Database filename
 * @return true if file exists and is readable, false otherwise
 */
bool fxdb_database_exists(const char* filename);

/**
 * Safely delete database file
 * @param filename Database filename
 * @return 0 on success, -1 on failure
 */
int fxdb_database_delete(const char* filename);

/* ============================================================================
 * File Locking Functions
 * ============================================================================ */

/**
 * Acquire exclusive lock on file
 * @param fd File descriptor
 * @return 0 on success, -1 on failure
 */
int fxdb_lock_file(int fd);

/**
 * Release lock on file
 * @param fd File descriptor
 * @return 0 on success, -1 on failure
 */
int fxdb_unlock_file(int fd);

#endif // FLEXON_IO_UTILS_H