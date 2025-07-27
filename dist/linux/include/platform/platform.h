#ifndef PLATFORM_PLATFORM_H
#define PLATFORM_PLATFORM_H

/* Platform detection macros */
#ifdef _WIN32
    #define FLEXON_PLATFORM_WINDOWS 1
    #ifdef _WIN64
        #define FLEXON_PLATFORM_WIN64 1
    #else
        #define FLEXON_PLATFORM_WIN32 1
    #endif
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #define FLEXON_PLATFORM_APPLE 1
    #if TARGET_OS_IPHONE
        #define FLEXON_PLATFORM_IOS 1
        #define FLEXON_PLATFORM_MOBILE 1
    #else
        #define FLEXON_PLATFORM_MACOS 1
    #endif
#elif defined(__ANDROID__)
    #define FLEXON_PLATFORM_ANDROID 1
    #define FLEXON_PLATFORM_MOBILE 1
#elif defined(__linux__)
    #define FLEXON_PLATFORM_LINUX 1
#elif defined(__unix__)
    #define FLEXON_PLATFORM_UNIX 1
#endif

/* Default values for undefined platforms */
#ifndef FLEXON_PLATFORM_WINDOWS
#define FLEXON_PLATFORM_WINDOWS 0
#endif

#ifndef FLEXON_PLATFORM_APPLE
#define FLEXON_PLATFORM_APPLE 0
#endif

#ifndef FLEXON_PLATFORM_MACOS
#define FLEXON_PLATFORM_MACOS 0
#endif

#ifndef FLEXON_PLATFORM_IOS
#define FLEXON_PLATFORM_IOS 0
#endif

#ifndef FLEXON_PLATFORM_ANDROID
#define FLEXON_PLATFORM_ANDROID 0
#endif

#ifndef FLEXON_PLATFORM_LINUX
#define FLEXON_PLATFORM_LINUX 0
#endif

#ifndef FLEXON_PLATFORM_MOBILE
#define FLEXON_PLATFORM_MOBILE 0
#endif

/* Architecture detection */
#if defined(_M_X64) || defined(__x86_64__)
    #define FLEXON_ARCH_X64 1
#elif defined(_M_IX86) || defined(__i386__)
    #define FLEXON_ARCH_X86 1
#elif defined(_M_ARM64) || defined(__aarch64__)
    #define FLEXON_ARCH_ARM64 1
#elif defined(_M_ARM) || defined(__arm__)
    #define FLEXON_ARCH_ARM 1
#endif

#endif /* PLATFORM_PLATFORM_H */