#ifndef FLEXON_COMPAT_H
#define FLEXON_COMPAT_H

/* ============================================================================
 * FlexonDB Function Compatibility and Shims
 * ============================================================================
 * This file provides compatibility shims for functions that may not be
 * available on all platforms, ensuring consistent API across platforms.
 */

#include "platform.h"

/* ============================================================================
 * String Function Compatibility
 * ============================================================================ */

/* strdup compatibility */
#ifdef FLEXON_NEED_STRDUP
char* flexon_strdup(const char* str);
#define strdup flexon_strdup
#endif

/* getline compatibility for platforms that don't have it */
#if defined(FLEXON_NEED_GETLINE)
ssize_t flexon_getline(char **lineptr, size_t *n, FILE *stream);
#define getline flexon_getline
#endif

/* snprintf compatibility for older MSVC */
#if defined(FLEXON_COMPILER_MSVC) && _MSC_VER < 1900
#define snprintf _snprintf
#endif

/* strcasecmp compatibility for Windows */
#if FLEXON_PLATFORM_WINDOWS
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

/* ============================================================================
 * File System Compatibility
 * ============================================================================ */

/* File access mode constants */
#if FLEXON_PLATFORM_WINDOWS
    #ifndef F_OK
        #define F_OK 0
        #define W_OK 2
        #define R_OK 4
    #endif
    #define access _access
    #define mkdir(path, mode) _mkdir(path)
    #define rmdir _rmdir
    #define unlink _unlink
#endif

/* Directory operations */
#if FLEXON_PLATFORM_WINDOWS
    #include <direct.h>
    #define getcwd _getcwd
    #define chdir _chdir
#endif

/* ============================================================================
 * Time Function Compatibility
 * ============================================================================ */

/* Ensure timespec is available */
#if !FLEXON_PLATFORM_WINDOWS
    /* For POSIX systems, this is in time.h which is included */
    #if FLEXON_HAVE_POSIX
        /* timespec should be available */
    #endif
#else
    /* Windows doesn't have clock_gettime, provide alternative */
    #ifndef CLOCK_REALTIME
        #define CLOCK_REALTIME 0
    #endif
    
    #ifndef _TIMESPEC_DEFINED
        struct timespec {
            time_t tv_sec;
            long tv_nsec;
        };
        #define _TIMESPEC_DEFINED
    #endif
    
    int flexon_clock_gettime(int clk_id, struct timespec *tp);
    #define clock_gettime flexon_clock_gettime
#endif

/* ============================================================================
 * Memory Mapping Compatibility
 * ============================================================================ */

#if FLEXON_HAVE_POSIX
    #include <sys/mman.h>
    #define FLEXON_HAVE_MMAP
#endif

#if FLEXON_PLATFORM_WINDOWS
    /* Windows memory mapping using CreateFileMapping */
    #define PROT_READ   1
    #define PROT_WRITE  2
    #define MAP_SHARED  1
    #define MAP_PRIVATE 2
    #define MAP_FAILED  ((void*)-1)
    
    void* flexon_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
    int flexon_munmap(void *addr, size_t length);
    
    #define mmap flexon_mmap
    #define munmap flexon_munmap
#endif

/* ============================================================================
 * Readline Compatibility
 * ============================================================================ */

/* Only include readline headers if we know they exist */
#ifdef FLEXON_HAVE_GNU_READLINE
    /* Only include if the macro is explicitly defined via CMake or feature detection */
    #if defined(FLEXON_READLINE_HEADERS_AVAILABLE)
        #include <readline/readline.h>
        #include <readline/history.h>
        #define FLEXON_READLINE_AVAILABLE
    #endif
#elif defined(FLEXON_HAVE_LIBEDIT_READLINE)
    /* Only include if the macro is explicitly defined via CMake or feature detection */
    #if defined(FLEXON_LIBEDIT_HEADERS_AVAILABLE)
        #include <editline/readline.h>
        #define FLEXON_READLINE_AVAILABLE
        /* Note: libedit doesn't have rl_replace_line */
        #ifndef rl_replace_line
            void flexon_rl_replace_line_stub(const char* text, int clear_undo);
            #define rl_replace_line flexon_rl_replace_line_stub
        #endif
    #endif
#endif

/* If no readline headers were included, provide stubs */
#ifndef FLEXON_READLINE_AVAILABLE
    /* Provide minimal readline stubs for platforms without readline */
    extern char* rl_readline_name;
    
    char* flexon_readline_stub(const char* prompt);
    void flexon_add_history_stub(const char* line);
    void flexon_rl_on_new_line_stub(void);
    void flexon_rl_replace_line_stub(const char* text, int clear_undo);
    void flexon_rl_redisplay_stub(void);
    void flexon_using_history_stub(void);
    int flexon_read_history_stub(const char* filename);
    int flexon_write_history_stub(const char* filename);
    int flexon_history_truncate_file_stub(const char* filename, int nlines);
    
    #define readline flexon_readline_stub
    #define add_history flexon_add_history_stub
    #define rl_on_new_line flexon_rl_on_new_line_stub
    #define rl_replace_line flexon_rl_replace_line_stub
    #define rl_redisplay flexon_rl_redisplay_stub
    #define using_history flexon_using_history_stub
    #define read_history flexon_read_history_stub
    #define write_history flexon_write_history_stub
    #define history_truncate_file flexon_history_truncate_file_stub
#endif

/* ============================================================================
 * Signal Handling Compatibility
 * ============================================================================ */

#if FLEXON_HAVE_POSIX
    #define FLEXON_HAVE_SIGNALS
#elif defined(FLEXON_PLATFORM_WINDOWS)
    /* Windows has different signal handling */
    #include <signal.h>
    #define FLEXON_HAVE_SIGNALS
    /* Note: Windows doesn't have SIGINT in the same way */
    #ifndef SIGINT
        #define SIGINT 2
    #endif
#endif

/* ============================================================================
 * Safe String Operations
 * ============================================================================ */

/* Provide safe string copy that works across platforms */
static inline int flexon_safe_strcpy(char* dest, size_t dest_size, const char* src) {
    if (!dest || !src || dest_size == 0) return -1;
    
#ifdef FLEXON_COMPILER_MSVC
    return strcpy_s(dest, dest_size, src) == 0 ? 0 : -1;
#else
    size_t src_len = strlen(src);
    if (src_len >= dest_size) return -1;
    strcpy(dest, src);
    return 0;
#endif
}

/* Provide safe string concatenation */
static inline int flexon_safe_strcat(char* dest, size_t dest_size, const char* src) {
    if (!dest || !src || dest_size == 0) return -1;
    
#ifdef FLEXON_COMPILER_MSVC
    return strcat_s(dest, dest_size, src) == 0 ? 0 : -1;
#else
    size_t dest_len = strlen(dest);
    size_t src_len = strlen(src);
    if (dest_len + src_len >= dest_size) return -1;
    strcat(dest, src);
    return 0;
#endif
}

/* ============================================================================
 * Feature Availability Macros
 * ============================================================================ */

/* Indicate what features are available on this platform */
#if FLEXON_HAVE_POSIX
    #define FLEXON_FEATURE_POSIX_IO
    #define FLEXON_FEATURE_POSIX_SIGNALS
#endif

#ifdef FLEXON_HAVE_MMAP
    #define FLEXON_FEATURE_MEMORY_MAPPING
#endif

#ifdef FLEXON_READLINE_AVAILABLE
    #define FLEXON_FEATURE_READLINE
#endif

#if defined(FLEXON_PLATFORM_DESKTOP)
    #define FLEXON_FEATURE_INTERACTIVE_SHELL
#endif

/* ============================================================================
 * Function Declarations for Implementation
 * ============================================================================ */

/* These will be implemented in src/compat/ */
#ifdef FLEXON_NEED_STRDUP
char* flexon_strdup(const char* str);
#endif

#if defined(FLEXON_NEED_GETLINE)
ssize_t flexon_getline(char **lineptr, size_t *n, FILE *stream);
#endif

#if FLEXON_PLATFORM_WINDOWS
int flexon_clock_gettime(int clk_id, struct timespec *tp);
void* flexon_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int flexon_munmap(void *addr, size_t length);
#endif

#ifdef FLEXON_HAVE_LIBEDIT_READLINE
void flexon_rl_replace_line_stub(const char* text, int clear_undo);
#endif

#ifndef FLEXON_READLINE_AVAILABLE
char* flexon_readline_stub(const char* prompt);
void flexon_add_history_stub(const char* line);
void flexon_rl_on_new_line_stub(void);
void flexon_rl_replace_line_stub(const char* text, int clear_undo);
void flexon_rl_redisplay_stub(void);
void flexon_using_history_stub(void);
int flexon_read_history_stub(const char* filename);
int flexon_write_history_stub(const char* filename);
int flexon_history_truncate_file_stub(const char* filename, int nlines);
#endif

#endif /* FLEXON_COMPAT_H */