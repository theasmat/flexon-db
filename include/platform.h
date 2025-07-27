#ifndef FLEXON_PLATFORM_H
#define FLEXON_PLATFORM_H

/* ============================================================================
 * FlexonDB Cross-Platform Compatibility Header
 * ============================================================================
 * This file handles platform detection and provides unified platform macros
 * for cross-platform compatibility across Linux, macOS, Windows, Android, iOS
 */

/* ============================================================================
 * Feature Test Macros (must be before any includes)
 * ============================================================================ */

/* Enable POSIX functionality */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

/* Enable GNU extensions on Linux (for additional features) */
#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

/* Enable BSD extensions on BSD systems */
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#define _BSD_SOURCE
#endif

/* ============================================================================
 * Platform Detection
 * ============================================================================ */

/* Operating System Detection */
#if defined(_WIN32) || defined(_WIN64)
    #define FLEXON_PLATFORM_WINDOWS
    #if defined(_WIN64)
        #define FLEXON_PLATFORM_WIN64
    #else
        #define FLEXON_PLATFORM_WIN32
    #endif
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #define FLEXON_PLATFORM_APPLE
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        #define FLEXON_PLATFORM_IOS
    #else
        #define FLEXON_PLATFORM_MACOS
    #endif
#elif defined(__ANDROID__)
    #define FLEXON_PLATFORM_ANDROID
    #define FLEXON_PLATFORM_LINUX_BASED
#elif defined(__linux__)
    #define FLEXON_PLATFORM_LINUX
    #define FLEXON_PLATFORM_LINUX_BASED
#elif defined(__unix__) || defined(__unix)
    #define FLEXON_PLATFORM_UNIX
#endif

/* Compiler Detection */
#if defined(_MSC_VER)
    #define FLEXON_COMPILER_MSVC
#elif defined(__MINGW32__) || defined(__MINGW64__)
    #define FLEXON_COMPILER_MINGW
#elif defined(__GNUC__)
    #define FLEXON_COMPILER_GCC
#elif defined(__clang__)
    #define FLEXON_COMPILER_CLANG
#endif

/* ============================================================================
 * Feature Detection
 * ============================================================================ */

/* POSIX Support */
#if defined(FLEXON_PLATFORM_LINUX) || defined(FLEXON_PLATFORM_MACOS) || \
    defined(FLEXON_PLATFORM_ANDROID) || defined(FLEXON_PLATFORM_UNIX)
    #define FLEXON_HAVE_POSIX
#endif

/* Readline Support Detection */
#if defined(FLEXON_PLATFORM_LINUX) || defined(FLEXON_PLATFORM_UNIX)
    #define FLEXON_HAVE_GNU_READLINE
#elif defined(FLEXON_PLATFORM_MACOS)
    #define FLEXON_HAVE_LIBEDIT_READLINE
#endif

/* Mobile Platform Detection */
#if defined(FLEXON_PLATFORM_ANDROID) || defined(FLEXON_PLATFORM_IOS)
    #define FLEXON_PLATFORM_MOBILE
#endif

/* Desktop Platform Detection */
#if defined(FLEXON_PLATFORM_LINUX) || defined(FLEXON_PLATFORM_MACOS) || \
    defined(FLEXON_PLATFORM_WINDOWS)
    #define FLEXON_PLATFORM_DESKTOP
#endif

/* ============================================================================
 * Standard Includes Based on Platform
 * ============================================================================ */

/* Universal includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* Platform-specific includes */
#ifdef FLEXON_PLATFORM_WINDOWS
    #include <windows.h>
    #include <io.h>
    #include <direct.h>
    #include <process.h>
#endif

#ifdef FLEXON_HAVE_POSIX
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <dirent.h>
    #include <signal.h>
    #include <time.h>
#endif

#ifdef FLEXON_PLATFORM_ANDROID
    #include <android/log.h>
#endif

#ifdef FLEXON_PLATFORM_IOS
    #include <os/log.h>
#endif

/* ============================================================================
 * Cross-Platform Logging Macros
 * ============================================================================ */

#ifdef FLEXON_PLATFORM_ANDROID
    #define FLEXON_LOG(...) __android_log_print(ANDROID_LOG_INFO, "FlexonDB", __VA_ARGS__)
    #define FLEXON_LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, "FlexonDB", __VA_ARGS__)
    #define FLEXON_LOG_DEBUG(...) __android_log_print(ANDROID_LOG_DEBUG, "FlexonDB", __VA_ARGS__)
#elif defined(FLEXON_PLATFORM_IOS)
    #define FLEXON_LOG(...) os_log(OS_LOG_DEFAULT, __VA_ARGS__)
    #define FLEXON_LOG_ERROR(...) os_log_error(OS_LOG_DEFAULT, __VA_ARGS__)
    #define FLEXON_LOG_DEBUG(...) os_log_debug(OS_LOG_DEFAULT, __VA_ARGS__)
#else
    #define FLEXON_LOG(...) printf(__VA_ARGS__)
    #define FLEXON_LOG_ERROR(...) fprintf(stderr, __VA_ARGS__)
    #define FLEXON_LOG_DEBUG(...) printf(__VA_ARGS__)
#endif

/* ============================================================================
 * Readline Availability Macros
 * ============================================================================ */

#if defined(FLEXON_HAVE_GNU_READLINE) || defined(FLEXON_HAVE_LIBEDIT_READLINE)
    #define FLEXON_HAVE_READLINE
#else
    #define FLEXON_NO_READLINE
#endif

/* ============================================================================
 * File Path Separators
 * ============================================================================ */

#ifdef FLEXON_PLATFORM_WINDOWS
    #define FLEXON_PATH_SEPARATOR '\\'
    #define FLEXON_PATH_SEPARATOR_STR "\\"
    #define FLEXON_PATH_LIST_SEPARATOR ';'
#else
    #define FLEXON_PATH_SEPARATOR '/'
    #define FLEXON_PATH_SEPARATOR_STR "/"
    #define FLEXON_PATH_LIST_SEPARATOR ':'
#endif

/* ============================================================================
 * Attribute Macros
 * ============================================================================ */

#ifdef FLEXON_COMPILER_MSVC
    #define FLEXON_UNUSED
    #define FLEXON_FORCE_INLINE __forceinline
    #define FLEXON_EXPORT __declspec(dllexport)
    #define FLEXON_IMPORT __declspec(dllimport)
#else
    #define FLEXON_UNUSED __attribute__((unused))
    #define FLEXON_FORCE_INLINE __attribute__((always_inline)) inline
    #define FLEXON_EXPORT __attribute__((visibility("default")))
    #define FLEXON_IMPORT
#endif

/* ============================================================================
 * Memory and String Function Availability
 * ============================================================================ */

#ifdef FLEXON_PLATFORM_WINDOWS
    #define FLEXON_NEED_STRDUP
    #define FLEXON_NEED_GETLINE
#endif

#ifdef FLEXON_PLATFORM_MOBILE
    #define FLEXON_MINIMAL_LIBC
#endif

#endif /* FLEXON_PLATFORM_H */