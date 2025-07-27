#include "common/colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#if FLEXON_PLATFORM_WINDOWS
    #include <windows.h>
    #include <io.h>
#else
    #include <unistd.h>
#endif

/* Global color state */
static int colors_enabled = -1; /* -1 = auto-detect, 0 = disabled, 1 = enabled */

int flexon_colors_supported(void) {
    if (colors_enabled != -1) {
        return colors_enabled;
    }

    /* Auto-detect color support */
    flexon_auto_detect_colors();
    return colors_enabled;
}

void flexon_set_colors_enabled(int enabled) {
    colors_enabled = enabled ? 1 : 0;
}

void flexon_auto_detect_colors(void) {
    /* Check environment variables first */
    const char* no_color = getenv("NO_COLOR");
    if (no_color && *no_color) {
        colors_enabled = 0;
        return;
    }

    const char* force_color = getenv("FORCE_COLOR");
    if (force_color && *force_color) {
        colors_enabled = 1;
        return;
    }

#if FLEXON_PLATFORM_WINDOWS
    /* Windows color detection */
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        colors_enabled = 0;
        return;
    }

    /* Check if stdout is a console */
    if (!_isatty(_fileno(stdout))) {
        colors_enabled = 0;
        return;
    }

    /* Try to enable ANSI escape sequences on Windows 10+ */
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (SetConsoleMode(hOut, dwMode)) {
            colors_enabled = 1;
            return;
        }
    }

    /* Fallback to no colors on older Windows */
    colors_enabled = 0;
#else
    /* Unix-like systems */
    
    /* Check if stdout is a terminal */
    if (!isatty(STDOUT_FILENO)) {
        colors_enabled = 0;
        return;
    }

    /* Check TERM environment variable */
    const char* term = getenv("TERM");
    if (!term) {
        colors_enabled = 0;
        return;
    }

    /* Check for known color-capable terminals */
    if (strstr(term, "color") != NULL ||
        strstr(term, "xterm") != NULL ||
        strstr(term, "screen") != NULL ||
        strstr(term, "tmux") != NULL ||
        strcmp(term, "linux") == 0 ||
        strcmp(term, "cygwin") == 0) {
        colors_enabled = 1;
        return;
    }

    /* Default to no colors for unknown terminals */
    colors_enabled = 0;
#endif
}

void flexon_print_colored(const char* color, const char* format, ...) {
    va_list args;
    
    /* Print color code if colors are enabled */
    if (flexon_colors_supported() && color && *color) {
        printf("%s", color);
    }
    
    /* Print the formatted message */
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    
    /* Print reset code if colors are enabled */
    if (flexon_colors_supported() && color && *color) {
        printf(ANSI_RESET);
    }
    
    fflush(stdout);
}

void flexon_print_colored_err(const char* color, const char* format, ...) {
    va_list args;
    
    /* Print color code if colors are enabled */
    if (flexon_colors_supported() && color && *color) {
        fprintf(stderr, "%s", color);
    }
    
    /* Print the formatted message */
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    /* Print reset code if colors are enabled */
    if (flexon_colors_supported() && color && *color) {
        fprintf(stderr, ANSI_RESET);
    }
    
    fflush(stderr);
}