/* ============================================================================
 * FlexonDB Compatibility Implementation
 * ============================================================================
 * This file implements compatibility shims for functions that may not be
 * available on all platforms.
 */

#include "../../include/compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * String Function Implementations
 * ============================================================================ */

#ifdef FLEXON_NEED_STRDUP
char* flexon_strdup(const char* str) {
    if (!str) return NULL;
    
    size_t len = strlen(str) + 1;
    char* copy = malloc(len);
    if (!copy) return NULL;
    
    memcpy(copy, str, len);
    return copy;
}
#endif

#ifdef FLEXON_NEED_GETLINE
ssize_t flexon_getline(char **lineptr, size_t *n, FILE *stream) {
    if (!lineptr || !n || !stream) return -1;
    
    size_t capacity = *n;
    char* buffer = *lineptr;
    
    if (!buffer || capacity == 0) {
        capacity = 128;
        buffer = malloc(capacity);
        if (!buffer) return -1;
    }
    
    size_t len = 0;
    int c;
    
    while ((c = fgetc(stream)) != EOF) {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char* new_buffer = realloc(buffer, capacity);
            if (!new_buffer) {
                if (buffer != *lineptr) free(buffer);
                return -1;
            }
            buffer = new_buffer;
        }
        
        buffer[len++] = c;
        if (c == '\n') break;
    }
    
    if (len == 0 && c == EOF) {
        if (buffer != *lineptr) free(buffer);
        return -1;
    }
    
    buffer[len] = '\0';
    *lineptr = buffer;
    *n = capacity;
    
    return len;
}
#endif

/* ============================================================================
 * Windows-Specific Implementations
 * ============================================================================ */

#ifdef FLEXON_PLATFORM_WINDOWS

int flexon_clock_gettime(int clk_id, struct timespec *tp) {
    (void)clk_id; /* Unused parameter */
    
    if (!tp) return -1;
    
    FILETIME ft;
    ULARGE_INTEGER ui;
    GetSystemTimeAsFileTime(&ft);
    
    ui.LowPart = ft.dwLowDateTime;
    ui.HighPart = ft.dwHighDateTime;
    
    /* Convert from 100ns intervals since 1601 to seconds since 1970 */
    tp->tv_sec = (time_t)((ui.QuadPart - 116444736000000000ULL) / 10000000ULL);
    tp->tv_nsec = (long)((ui.QuadPart % 10000000ULL) * 100);
    
    return 0;
}

void* flexon_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    (void)addr;   /* Unused parameters */
    (void)prot;
    (void)flags;
    
    HANDLE hFile = (HANDLE)_get_osfhandle(fd);
    if (hFile == INVALID_HANDLE_VALUE) return MAP_FAILED;
    
    HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (!hMapping) return MAP_FAILED;
    
    void* mapped = MapViewOfFile(hMapping, FILE_MAP_READ, 
                                 (DWORD)(offset >> 32), 
                                 (DWORD)(offset & 0xFFFFFFFF), 
                                 length);
    
    CloseHandle(hMapping);
    return mapped ? mapped : MAP_FAILED;
}

int flexon_munmap(void *addr, size_t length) {
    (void)length; /* Unused parameter */
    return UnmapViewOfFile(addr) ? 0 : -1;
}

#endif

/* ============================================================================
 * macOS libedit Compatibility
 * ============================================================================ */

#ifdef FLEXON_HAVE_LIBEDIT_READLINE
void flexon_rl_replace_line_stub(const char* text, int clear_undo) {
    /* libedit doesn't have rl_replace_line, so this is a no-op */
    (void)text;
    (void)clear_undo;
}
#endif

/* ============================================================================
 * Readline Stub Implementation for Mobile/No-Readline Platforms
 * ============================================================================ */

#ifndef FLEXON_READLINE_AVAILABLE

#define MAX_LINE_LENGTH 1024

/* Global variables for readline compatibility */
char* rl_readline_name = NULL;

char* flexon_readline_stub(const char* prompt) {
    if (prompt) {
        printf("%s", prompt);
        fflush(stdout);
    }
    
    char* line = malloc(MAX_LINE_LENGTH);
    if (!line) return NULL;
    
    if (fgets(line, MAX_LINE_LENGTH, stdin)) {
        /* Remove trailing newline if present */
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        return line;
    }
    
    free(line);
    return NULL;
}

void flexon_add_history_stub(const char* line) {
    /* No-op for stub implementation */
    (void)line;
}

void flexon_rl_on_new_line_stub(void) {
    /* No-op for stub implementation */
}

void flexon_rl_replace_line_stub(const char* text, int clear_undo) {
    /* No-op for stub implementation */
    (void)text;
    (void)clear_undo;
}

void flexon_rl_redisplay_stub(void) {
    /* No-op for stub implementation */
}

void flexon_using_history_stub(void) {
    /* No-op for stub implementation */
}

int flexon_read_history_stub(const char* filename) {
    /* No-op for stub implementation */
    (void)filename;
    return 0;
}

int flexon_write_history_stub(const char* filename) {
    /* No-op for stub implementation */
    (void)filename;
    return 0;
}

int flexon_history_truncate_file_stub(const char* filename, int nlines) {
    /* No-op for stub implementation */
    (void)filename;
    (void)nlines;
    return 0;
}

#endif