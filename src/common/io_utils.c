#define _DEFAULT_SOURCE
#include "../../include/io_utils.h"
#include "../../include/error.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

/* ============================================================================
 * Buffered Writing Implementation
 * ============================================================================ */

/**
 * Create a new buffered writer for the specified file
 */
fxdb_buffered_writer_t* fxdb_buffered_writer_create(const char* filename, bool lock_file) {
    if (!filename) {
        return NULL;
    }

    fxdb_buffered_writer_t* writer = calloc(1, sizeof(fxdb_buffered_writer_t));
    if (!writer) {
        return NULL;
    }

    // Open file for writing
    writer->file = fopen(filename, "wb");
    if (!writer->file) {
        free(writer);
        return NULL;
    }

    // Get file descriptor for locking
    writer->lock_fd = fileno(writer->file);
    
    // Acquire file lock if requested
    if (lock_file) {
        if (fxdb_lock_file(writer->lock_fd) == 0) {
            writer->is_locked = true;
        } else {
            // Lock failed, but continue without lock
            writer->is_locked = false;
        }
    }

    writer->buffer_pos = 0;
    writer->total_written = 0;

    return writer;
}

/**
 * Write uint32_t value to buffered writer
 */
int fxdb_write_uint32(fxdb_buffered_writer_t* writer, uint32_t value) {
    if (!writer) {
        return -1;
    }

    return fxdb_write_data(writer, &value, sizeof(uint32_t));
}

/**
 * Write string to buffered writer with length prefix
 */
int fxdb_write_string(fxdb_buffered_writer_t* writer, const char* str, size_t max_len) {
    if (!writer || !str) {
        return -1;
    }

    size_t len = strlen(str);
    if (len > max_len) {
        len = max_len;
    }

    // Write length prefix
    uint32_t str_len = (uint32_t)len;
    if (fxdb_write_uint32(writer, str_len) != 0) {
        return -1;
    }

    // Write string data
    if (len > 0) {
        if (fxdb_write_data(writer, str, len) != 0) {
            return -1;
        }
    }

    return 0;
}

/**
 * Write raw data to buffered writer
 */
int fxdb_write_data(fxdb_buffered_writer_t* writer, const void* data, size_t size) {
    if (!writer || !data || size == 0) {
        return -1;
    }

    const char* bytes = (const char*)data;
    size_t remaining = size;

    while (remaining > 0) {
        // Calculate space available in buffer
        size_t space_available = FXDB_BUFFER_SIZE - writer->buffer_pos;
        
        if (space_available == 0) {
            // Buffer is full, flush it
            if (fxdb_flush_buffer(writer) != 0) {
                return -1;
            }
            space_available = FXDB_BUFFER_SIZE;
        }

        // Copy as much as possible to buffer
        size_t to_copy = (remaining < space_available) ? remaining : space_available;
        memcpy(writer->buffer + writer->buffer_pos, bytes, to_copy);
        
        writer->buffer_pos += to_copy;
        bytes += to_copy;
        remaining -= to_copy;
    }

    return 0;
}

/**
 * Flush buffered writer to disk
 */
int fxdb_flush_buffer(fxdb_buffered_writer_t* writer) {
    if (!writer || !writer->file) {
        return -1;
    }

    if (writer->buffer_pos == 0) {
        return 0; // Nothing to flush
    }

    size_t written = fwrite(writer->buffer, 1, writer->buffer_pos, writer->file);
    if (written != writer->buffer_pos) {
        return -1;
    }

    // Ensure data is written to disk
    if (fflush(writer->file) != 0) {
        return -1;
    }

    writer->total_written += writer->buffer_pos;
    writer->buffer_pos = 0;

    return 0;
}

/**
 * Close buffered writer and release resources
 */
int fxdb_buffered_writer_close(fxdb_buffered_writer_t* writer) {
    if (!writer) {
        return -1;
    }

    int result = 0;

    // Flush any remaining data
    if (fxdb_flush_buffer(writer) != 0) {
        result = -1;
    }

    // Release file lock
    if (writer->is_locked) {
        fxdb_unlock_file(writer->lock_fd);
    }

    // Close file
    if (writer->file) {
        if (fclose(writer->file) != 0) {
            result = -1;
        }
    }

    free(writer);
    return result;
}

/* ============================================================================
 * Memory-Mapped Reading Implementation
 * ============================================================================ */

/**
 * Create a memory-mapped reader for the specified file
 */
fxdb_mmap_reader_t* fxdb_mmap_reader_open(const char* filename) {
    if (!filename) {
        return NULL;
    }

    fxdb_mmap_reader_t* reader = calloc(1, sizeof(fxdb_mmap_reader_t));
    if (!reader) {
        return NULL;
    }

    // Open file for reading
    reader->fd = open(filename, O_RDONLY);
    if (reader->fd == -1) {
        free(reader);
        return NULL;
    }

    // Get file size
    struct stat st;
    if (fstat(reader->fd, &st) == -1) {
        close(reader->fd);
        free(reader);
        return NULL;
    }

    reader->file_size = st.st_size;

    // Only use mmap for files larger than minimum size
    if (reader->file_size >= FXDB_MIN_MMAP_SIZE) {
        // Memory map the file
        reader->mmap_data = mmap(NULL, reader->file_size, PROT_READ, MAP_SHARED, reader->fd, 0);
        if (reader->mmap_data == MAP_FAILED) {
            // Fallback: don't use mmap, keep file descriptor for regular reads
            reader->mmap_data = NULL;
            reader->is_mapped = false;
        } else {
            reader->is_mapped = true;
        }
    } else {
        reader->mmap_data = NULL;
        reader->is_mapped = false;
    }

    return reader;
}

/**
 * Read uint32_t value from memory-mapped file
 */
uint32_t fxdb_read_uint32_mmap(fxdb_mmap_reader_t* reader, size_t offset) {
    if (!reader) {
        return 0;
    }

    if (reader->is_mapped) {
        if (offset + sizeof(uint32_t) > reader->file_size) {
            return 0; // Out of bounds
        }
        return *(uint32_t*)((char*)reader->mmap_data + offset);
    } else {
        // Fallback to regular file I/O
        if (lseek(reader->fd, offset, SEEK_SET) == -1) {
            return 0;
        }
        uint32_t value;
        if (read(reader->fd, &value, sizeof(uint32_t)) != sizeof(uint32_t)) {
            return 0;
        }
        return value;
    }
}

/**
 * Read string from memory-mapped file
 */
int fxdb_read_string_mmap(fxdb_mmap_reader_t* reader, size_t offset, char* out, size_t max_len) {
    if (!reader || !out || max_len == 0) {
        return -1;
    }

    // First read the length prefix
    uint32_t str_len = fxdb_read_uint32_mmap(reader, offset);
    if (str_len == 0) {
        out[0] = '\0';
        return 0;
    }

    size_t data_offset = offset + sizeof(uint32_t);
    size_t copy_len = (str_len < max_len - 1) ? str_len : max_len - 1;

    if (reader->is_mapped) {
        if (data_offset + str_len > reader->file_size) {
            return -1; // Out of bounds
        }
        memcpy(out, (char*)reader->mmap_data + data_offset, copy_len);
    } else {
        // Fallback to regular file I/O
        if (lseek(reader->fd, data_offset, SEEK_SET) == -1) {
            return -1;
        }
        if (read(reader->fd, out, copy_len) != (ssize_t)copy_len) {
            return -1;
        }
    }

    out[copy_len] = '\0';
    return (int)copy_len;
}

/**
 * Get pointer to data at offset in memory-mapped file
 */
void* fxdb_mmap_get_ptr(fxdb_mmap_reader_t* reader, size_t offset) {
    if (!reader || !reader->is_mapped) {
        return NULL;
    }

    if (offset >= reader->file_size) {
        return NULL; // Out of bounds
    }

    return (char*)reader->mmap_data + offset;
}

/**
 * Close memory-mapped reader and release resources
 */
void fxdb_mmap_reader_close(fxdb_mmap_reader_t* reader) {
    if (!reader) {
        return;
    }

    // Unmap memory if mapped
    if (reader->is_mapped && reader->mmap_data) {
        munmap(reader->mmap_data, reader->file_size);
    }

    // Close file descriptor
    if (reader->fd != -1) {
        close(reader->fd);
    }

    free(reader);
}

/* ============================================================================
 * File Management Implementation
 * ============================================================================ */

/**
 * Normalize filename to ensure .fxdb extension
 */
char* fxdb_normalize_filename(const char* input) {
    if (!input) {
        return NULL;
    }

    size_t input_len = strlen(input);
    if (input_len == 0) {
        return NULL;
    }

    // Check if already has .fxdb extension
    if (fxdb_has_extension(input)) {
        return strdup(input);
    }

    // Check if has .db extension and replace it
    const char* ext = strrchr(input, '.');
    if (ext && strcmp(ext, ".db") == 0) {
        // Replace .db with .fxdb
        size_t base_len = ext - input;
        char* result = malloc(base_len + strlen(FXDB_EXT) + 1);
        if (!result) {
            return NULL;
        }
        memcpy(result, input, base_len);
        strcpy(result + base_len, FXDB_EXT);
        return result;
    }

    // Add .fxdb extension
    char* result = malloc(input_len + strlen(FXDB_EXT) + 1);
    if (!result) {
        return NULL;
    }
    strcpy(result, input);
    strcat(result, FXDB_EXT);
    return result;
}

/**
 * Check if filename has .fxdb extension
 */
bool fxdb_has_extension(const char* filename) {
    if (!filename) {
        return false;
    }

    size_t len = strlen(filename);
    size_t ext_len = strlen(FXDB_EXT);

    if (len < ext_len) {
        return false;
    }

    return strcmp(filename + len - ext_len, FXDB_EXT) == 0;
}

/**
 * Check if database file exists
 */
bool fxdb_database_exists(const char* filename) {
    if (!filename) {
        return false;
    }

    struct stat st;
    return (stat(filename, &st) == 0) && S_ISREG(st.st_mode);
}

/**
 * Safely delete database file
 */
int fxdb_database_delete(const char* filename) {
    if (!filename) {
        return -1;
    }

    // Check if file exists first
    if (!fxdb_database_exists(filename)) {
        return -1; // File doesn't exist
    }

    return unlink(filename);
}

/* ============================================================================
 * File Locking Implementation
 * ============================================================================ */

/**
 * Acquire exclusive lock on file
 */
int fxdb_lock_file(int fd) {
    if (fd < 0) {
        return -1;
    }

    struct flock lock;
    lock.l_type = F_WRLCK;    // Exclusive write lock
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;           // Lock entire file

    return fcntl(fd, F_SETLK, &lock);
}

/**
 * Release lock on file
 */
int fxdb_unlock_file(int fd) {
    if (fd < 0) {
        return -1;
    }

    struct flock lock;
    lock.l_type = F_UNLCK;    // Unlock
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;           // Unlock entire file

    return fcntl(fd, F_SETLK, &lock);
}