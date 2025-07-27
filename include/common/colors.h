#ifndef COMMON_COLORS_H
#define COMMON_COLORS_H

#include "../platform.h"

/* ANSI Color Support with Platform Detection */

/* Determine color support */
#if FLEXON_PLATFORM_WINDOWS
    /* Windows 10+ supports ANSI, but we'll be conservative */
    #ifndef FLEXON_FORCE_COLORS
        #define FLEXON_COLOR_SUPPORT 0
    #else
        #define FLEXON_COLOR_SUPPORT 1
    #endif
#else
    /* Unix-like systems generally support ANSI colors */
    #define FLEXON_COLOR_SUPPORT 1
#endif

/* Color macros - defined as empty strings if no color support */
#if FLEXON_COLOR_SUPPORT

/* Reset and formatting */
#define ANSI_RESET      "\033[0m"
#define ANSI_BOLD       "\033[1m"
#define ANSI_DIM        "\033[2m"
#define ANSI_ITALIC     "\033[3m"
#define ANSI_UNDERLINE  "\033[4m"
#define ANSI_BLINK      "\033[5m"
#define ANSI_REVERSE    "\033[7m"
#define ANSI_STRIKETHROUGH "\033[9m"

/* Standard colors */
#define ANSI_BLACK      "\033[30m"
#define ANSI_RED        "\033[31m"
#define ANSI_GREEN      "\033[32m"
#define ANSI_YELLOW     "\033[33m"
#define ANSI_BLUE       "\033[34m"
#define ANSI_MAGENTA    "\033[35m"
#define ANSI_CYAN       "\033[36m"
#define ANSI_WHITE      "\033[37m"

/* Bright colors */
#define ANSI_BR_BLACK   "\033[90m"
#define ANSI_BR_RED     "\033[91m"
#define ANSI_BR_GREEN   "\033[92m"
#define ANSI_BR_YELLOW  "\033[93m"
#define ANSI_BR_BLUE    "\033[94m"
#define ANSI_BR_MAGENTA "\033[95m"
#define ANSI_BR_CYAN    "\033[96m"
#define ANSI_BR_WHITE   "\033[97m"

/* Background colors */
#define ANSI_BG_BLACK   "\033[40m"
#define ANSI_BG_RED     "\033[41m"
#define ANSI_BG_GREEN   "\033[42m"
#define ANSI_BG_YELLOW  "\033[43m"
#define ANSI_BG_BLUE    "\033[44m"
#define ANSI_BG_MAGENTA "\033[45m"
#define ANSI_BG_CYAN    "\033[46m"
#define ANSI_BG_WHITE   "\033[47m"

/* Bright background colors */
#define ANSI_BG_BR_BLACK   "\033[100m"
#define ANSI_BG_BR_RED     "\033[101m"
#define ANSI_BG_BR_GREEN   "\033[102m"
#define ANSI_BG_BR_YELLOW  "\033[103m"
#define ANSI_BG_BR_BLUE    "\033[104m"
#define ANSI_BG_BR_MAGENTA "\033[105m"
#define ANSI_BG_BR_CYAN    "\033[106m"
#define ANSI_BG_BR_WHITE   "\033[107m"

#else /* No color support */

/* Reset and formatting */
#define ANSI_RESET      ""
#define ANSI_BOLD       ""
#define ANSI_DIM        ""
#define ANSI_ITALIC     ""
#define ANSI_UNDERLINE  ""
#define ANSI_BLINK      ""
#define ANSI_REVERSE    ""
#define ANSI_STRIKETHROUGH ""

/* Standard colors */
#define ANSI_BLACK      ""
#define ANSI_RED        ""
#define ANSI_GREEN      ""
#define ANSI_YELLOW     ""
#define ANSI_BLUE       ""
#define ANSI_MAGENTA    ""
#define ANSI_CYAN       ""
#define ANSI_WHITE      ""

/* Bright colors */
#define ANSI_BR_BLACK   ""
#define ANSI_BR_RED     ""
#define ANSI_BR_GREEN   ""
#define ANSI_BR_YELLOW  ""
#define ANSI_BR_BLUE    ""
#define ANSI_BR_MAGENTA ""
#define ANSI_BR_CYAN    ""
#define ANSI_BR_WHITE   ""

/* Background colors */
#define ANSI_BG_BLACK   ""
#define ANSI_BG_RED     ""
#define ANSI_BG_GREEN   ""
#define ANSI_BG_YELLOW  ""
#define ANSI_BG_BLUE    ""
#define ANSI_BG_MAGENTA ""
#define ANSI_BG_CYAN    ""
#define ANSI_BG_WHITE   ""

/* Bright background colors */
#define ANSI_BG_BR_BLACK   ""
#define ANSI_BG_BR_RED     ""
#define ANSI_BG_BR_GREEN   ""
#define ANSI_BG_BR_YELLOW  ""
#define ANSI_BG_BR_BLUE    ""
#define ANSI_BG_BR_MAGENTA ""
#define ANSI_BG_BR_CYAN    ""
#define ANSI_BG_BR_WHITE   ""

#endif /* FLEXON_COLOR_SUPPORT */

/* Semantic color aliases */
#define COLOR_ERROR     ANSI_RED
#define COLOR_WARNING   ANSI_YELLOW
#define COLOR_SUCCESS   ANSI_GREEN
#define COLOR_INFO      ANSI_BLUE
#define COLOR_DEBUG     ANSI_MAGENTA
#define COLOR_EMPHASIS  ANSI_BOLD
#define COLOR_PROMPT    ANSI_BR_BLUE ANSI_BOLD
#define COLOR_DATABASE  ANSI_CYAN
#define COLOR_FIELD     ANSI_GREEN
#define COLOR_VALUE     ANSI_WHITE
#define COLOR_COMMAND   ANSI_BR_YELLOW

/* Functions for dynamic color control */

/**
 * Check if colors are supported/enabled
 * @return 1 if colors are supported, 0 otherwise
 */
int flexon_colors_supported(void);

/**
 * Enable or disable color output
 * @param enabled 1 to enable, 0 to disable
 */
void flexon_set_colors_enabled(int enabled);

/**
 * Auto-detect color support based on environment
 * Sets color state based on terminal capabilities and environment variables
 */
void flexon_auto_detect_colors(void);

/**
 * Print colored text to stdout
 * @param color Color code (e.g., ANSI_RED)
 * @param format Printf-style format string
 * @param ... Printf-style arguments
 */
void flexon_print_colored(const char* color, const char* format, ...);

/**
 * Print colored text to stderr
 * @param color Color code (e.g., ANSI_RED)
 * @param format Printf-style format string
 * @param ... Printf-style arguments
 */
void flexon_print_colored_err(const char* color, const char* format, ...);

#endif /* COMMON_COLORS_H */