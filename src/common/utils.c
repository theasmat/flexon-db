#include "../../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

/* ============================================================================
 * File Utilities
 * ============================================================================ */

const char* utils_get_file_extension(const char* path) {
    if (!path) return "";
    
    const char* dot = strrchr(path, '.');
    if (!dot || dot == path) return "";
    return dot;
}

const char* utils_get_filename(const char* path) {
    if (!path) return "";
    
    const char* slash = strrchr(path, '/');
    if (!slash) return path;
    return slash + 1;
}

void utils_join_path(const char* dir, const char* filename, char* result, size_t result_size) {
    if (!dir || !filename || !result || result_size == 0) return;
    
    if (strlen(dir) == 0) {
        utils_safe_strcpy(result, filename, result_size);
        return;
    }
    
    snprintf(result, result_size, "%s/%s", dir, filename);
}

bool utils_file_exists(const char* path) {
    if (!path) return false;
    return access(path, F_OK) == 0;
}

uint64_t utils_get_file_size(const char* path) {
    if (!path) return 0;
    
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    return st.st_size;
}

time_t utils_get_file_mtime(const char* path) {
    if (!path) return 0;
    
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    return st.st_mtime;
}

/* ============================================================================
 * String Utilities
 * ============================================================================ */

void utils_format_file_size(uint64_t bytes, char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) return;
    
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    double size = (double)bytes;
    
    while (size >= 1024.0 && unit_index < 4) {
        size /= 1024.0;
        unit_index++;
    }
    
    if (unit_index == 0) {
        snprintf(buffer, buffer_size, "%llu %s", (unsigned long long)bytes, units[unit_index]);
    } else {
        snprintf(buffer, buffer_size, "%.1f %s", size, units[unit_index]);
    }
}

void utils_format_timestamp(time_t timestamp, char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) return;
    
    struct tm* tm_info = localtime(&timestamp);
    if (!tm_info) {
        utils_safe_strcpy(buffer, "Invalid time", buffer_size);
        return;
    }
    
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", tm_info);
}

char* utils_trim_string(char* str) {
    if (!str) return NULL;
    
    // Trim leading whitespace
    while (isspace((unsigned char)*str)) str++;
    
    if (*str == 0) return str; // All spaces
    
    // Trim trailing whitespace
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    
    return str;
}

void utils_string_to_lower(char* str) {
    if (!str) return;
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

bool utils_string_is_empty(const char* str) {
    if (!str) return true;
    while (*str) {
        if (!isspace((unsigned char)*str)) return false;
        str++;
    }
    return true;
}

void utils_safe_strcpy(char* dest, const char* src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) return;
    
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

bool utils_validate_field_name(const char* name) {
    if (!name || strlen(name) == 0) return false;
    
    // Must start with letter or underscore
    if (!isalpha((unsigned char)name[0]) && name[0] != '_') return false;
    
    // Rest must be alphanumeric or underscore
    for (size_t i = 1; i < strlen(name); i++) {
        if (!isalnum((unsigned char)name[i]) && name[i] != '_') return false;
    }
    
    return strlen(name) <= MAX_FIELD_NAME_LENGTH;
}

bool utils_validate_database_name(const char* name) {
    if (!name || strlen(name) == 0) return false;
    
    // Check for invalid characters
    const char* invalid_chars = "/\\:*?\"<>|";
    if (strpbrk(name, invalid_chars) != NULL) return false;
    
    // Check length
    return strlen(name) <= 255;
}

/* ============================================================================
 * Memory Utilities
 * ============================================================================ */

void* utils_malloc(size_t size, error_context_t* error) {
    void* ptr = malloc(size);
    if (!ptr && error) {
        SET_ERROR(error, ERROR_MEMORY, errno, "Failed to allocate %zu bytes", size);
    }
    return ptr;
}

void* utils_calloc(size_t count, size_t size, error_context_t* error) {
    void* ptr = calloc(count, size);
    if (!ptr && error) {
        SET_ERROR(error, ERROR_MEMORY, errno, "Failed to allocate %zu x %zu bytes", count, size);
    }
    return ptr;
}

void utils_free(void** ptr) {
    if (ptr && *ptr) {
        free(*ptr);
        *ptr = NULL;
    }
}

char* utils_strdup(const char* str, error_context_t* error) {
    if (!str) return NULL;
    
    size_t len = strlen(str) + 1;
    char* copy = utils_malloc(len, error);
    if (copy) {
        strcpy(copy, str);
    }
    return copy;
}

/* ============================================================================
 * Time Utilities
 * ============================================================================ */

uint64_t utils_get_timestamp(void) {
    return (uint64_t)time(NULL);
}

static void utils_get_time(struct timespec* ts) {
    if (!ts) return;
    clock_gettime(CLOCK_MONOTONIC, ts);
}

void utils_timing_start(timing_info_enhanced_t* timing) {
    if (!timing) return;
    struct timespec ts;
    utils_get_time(&ts);
    timing->start_time = ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

double utils_timing_end(timing_info_enhanced_t* timing) {
    if (!timing) return 0.0;
    struct timespec ts;
    utils_get_time(&ts);
    timing->end_time = ts.tv_sec * 1000000000ULL + ts.tv_nsec;
    timing->elapsed_ms = (timing->end_time - timing->start_time) / 1000000.0;
    return timing->elapsed_ms;
}

/* ============================================================================
 * Platform Utilities
 * ============================================================================ */

void utils_get_current_user(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) return;
    
    const char* user = getenv("USER");
    if (!user) {
        struct passwd* pw = getpwuid(getuid());
        user = pw ? pw->pw_name : "unknown";
    }
    
    utils_safe_strcpy(buffer, user, buffer_size);
}

void utils_get_current_directory(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) return;
    
    if (!getcwd(buffer, buffer_size)) {
        utils_safe_strcpy(buffer, ".", buffer_size);
    }
}

const char* utils_getenv(const char* name, const char* default_value) {
    const char* value = getenv(name);
    return value ? value : default_value;
}

/* ============================================================================
 * Data Conversion Utilities
 * ============================================================================ */

bool utils_str_to_int32(const char* str, int32_t* result) {
    if (!str || !result) return false;
    
    char* endptr;
    long val = strtol(str, &endptr, 10);
    
    if (endptr == str || *endptr != '\0') return false;
    if (val < INT32_MIN || val > INT32_MAX) return false;
    
    *result = (int32_t)val;
    return true;
}

bool utils_str_to_float(const char* str, float* result) {
    if (!str || !result) return false;
    
    char* endptr;
    float val = strtof(str, &endptr);
    
    if (endptr == str || *endptr != '\0') return false;
    
    *result = val;
    return true;
}

bool utils_str_to_bool(const char* str, bool* result) {
    if (!str || !result) return false;
    
    if (strcasecmp(str, "true") == 0 || strcmp(str, "1") == 0) {
        *result = true;
        return true;
    }
    if (strcasecmp(str, "false") == 0 || strcmp(str, "0") == 0) {
        *result = false;
        return true;
    }
    
    return false;
}

const char* utils_field_type_to_string(field_type_t type) {
    switch (type) {
        case TYPE_INT32:    return "int32";
        case TYPE_FLOAT:    return "float";
        case TYPE_STRING:   return "string";
        case TYPE_BOOL:     return "bool";
        default:            return "unknown";
    }
}

field_type_t utils_string_to_field_type(const char* type_str) {
    if (!type_str) return TYPE_UNKNOWN;
    
    if (strcmp(type_str, "int32") == 0) return TYPE_INT32;
    if (strcmp(type_str, "float") == 0) return TYPE_FLOAT;
    if (strcmp(type_str, "string") == 0) return TYPE_STRING;
    if (strcmp(type_str, "bool") == 0) return TYPE_BOOL;
    
    return TYPE_UNKNOWN;
}

uint32_t utils_get_field_type_size(field_type_t type) {
    switch (type) {
        case TYPE_INT32:    return 4;
        case TYPE_FLOAT:    return 4;
        case TYPE_STRING:   return MAX_STRING_LENGTH;
        case TYPE_BOOL:     return 1;
        default:            return 0;
    }
}

/* ============================================================================
 * Checksum Utilities
 * ============================================================================ */

uint32_t utils_simple_checksum(const void* data, size_t length) {
    if (!data || length == 0) return 0;
    
    uint32_t checksum = 0;
    const uint8_t* bytes = (const uint8_t*)data;
    
    for (size_t i = 0; i < length; i++) {
        checksum += bytes[i];
        checksum = (checksum << 1) | (checksum >> 31); // Rotate left
    }
    
    return checksum;
}

bool utils_verify_checksum(const void* data, size_t length, uint32_t expected_checksum) {
    return utils_simple_checksum(data, length) == expected_checksum;
}