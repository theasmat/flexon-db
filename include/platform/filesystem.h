#ifndef PLATFORM_FILESYSTEM_H
#define PLATFORM_FILESYSTEM_H

#include "platform.h"
#include <stdio.h>

/* Filesystem abstraction layer */

/* Path separator */
#if FLEXON_PLATFORM_WINDOWS
    #define FLEXON_PATH_SEPARATOR '\\'
    #define FLEXON_PATH_SEPARATOR_STR "\\"
    #define FLEXON_PATH_LIST_SEPARATOR ';'
#else
    #define FLEXON_PATH_SEPARATOR '/'
    #define FLEXON_PATH_SEPARATOR_STR "/"
    #define FLEXON_PATH_LIST_SEPARATOR ':'
#endif

/* Maximum path length */
#if FLEXON_PLATFORM_WINDOWS
    #define FLEXON_MAX_PATH 260
#else
    #define FLEXON_MAX_PATH 4096
#endif

/* File attributes */
typedef struct {
    int is_directory;
    int is_regular_file;
    int is_readable;
    int is_writable;
    long long size;
    long long modified_time;
} flexon_file_info_t;

/**
 * Check if a file or directory exists
 * @param path Path to check
 * @return 1 if exists, 0 if not, -1 on error
 */
int flexon_file_exists(const char* path);

/**
 * Check if a path is a directory
 * @param path Path to check
 * @return 1 if directory, 0 if not, -1 on error
 */
int flexon_is_directory(const char* path);

/**
 * Check if a path is a regular file
 * @param path Path to check
 * @return 1 if regular file, 0 if not, -1 on error
 */
int flexon_is_regular_file(const char* path);

/**
 * Get file information
 * @param path Path to file
 * @param info Pointer to info structure to fill
 * @return 0 on success, -1 on error
 */
int flexon_get_file_info(const char* path, flexon_file_info_t* info);

/**
 * Create a directory
 * @param path Directory path to create
 * @return 0 on success, -1 on error
 */
int flexon_create_directory(const char* path);

/**
 * Create directory and any missing parent directories
 * @param path Directory path to create
 * @return 0 on success, -1 on error
 */
int flexon_create_directories(const char* path);

/**
 * Remove a file
 * @param path File path to remove
 * @return 0 on success, -1 on error
 */
int flexon_remove_file(const char* path);

/**
 * Remove a directory (must be empty)
 * @param path Directory path to remove
 * @return 0 on success, -1 on error
 */
int flexon_remove_directory(const char* path);

/**
 * Get current working directory
 * @param buffer Buffer to store path
 * @param size Size of buffer
 * @return 0 on success, -1 on error
 */
int flexon_get_current_directory(char* buffer, size_t size);

/**
 * Change current working directory
 * @param path New directory path
 * @return 0 on success, -1 on error
 */
int flexon_set_current_directory(const char* path);

/**
 * Join two path components
 * @param result Buffer to store result
 * @param result_size Size of result buffer
 * @param path1 First path component
 * @param path2 Second path component
 * @return 0 on success, -1 on error
 */
int flexon_path_join(char* result, size_t result_size, const char* path1, const char* path2);

/**
 * Get the directory part of a path
 * @param result Buffer to store result
 * @param result_size Size of result buffer
 * @param path Full path
 * @return 0 on success, -1 on error
 */
int flexon_path_dirname(char* result, size_t result_size, const char* path);

/**
 * Get the filename part of a path
 * @param result Buffer to store result
 * @param result_size Size of result buffer
 * @param path Full path
 * @return 0 on success, -1 on error
 */
int flexon_path_basename(char* result, size_t result_size, const char* path);

/**
 * Get the file extension
 * @param path File path
 * @return Pointer to extension (including dot), or empty string if no extension
 */
const char* flexon_path_extension(const char* path);

/**
 * Normalize a path (resolve . and .. components)
 * @param result Buffer to store result
 * @param result_size Size of result buffer
 * @param path Path to normalize
 * @return 0 on success, -1 on error
 */
int flexon_path_normalize(char* result, size_t result_size, const char* path);

/**
 * Convert path to absolute path
 * @param result Buffer to store result
 * @param result_size Size of result buffer
 * @param path Relative or absolute path
 * @return 0 on success, -1 on error
 */
int flexon_path_absolute(char* result, size_t result_size, const char* path);

/* Directory listing */
typedef struct flexon_dir_entry {
    char name[256];
    int is_directory;
    struct flexon_dir_entry* next;
} flexon_dir_entry_t;

/**
 * List directory contents
 * @param path Directory path
 * @return Linked list of directory entries, or NULL on error
 *         Caller must free using flexon_free_dir_list()
 */
flexon_dir_entry_t* flexon_list_directory(const char* path);

/**
 * Free directory listing
 * @param list Directory list returned by flexon_list_directory()
 */
void flexon_free_dir_list(flexon_dir_entry_t* list);

#endif /* PLATFORM_FILESYSTEM_H */