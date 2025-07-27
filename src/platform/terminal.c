#include "platform/terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if FLEXON_PLATFORM_WINDOWS
    #include <windows.h>
    #include <io.h>
#else
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <termios.h>
#endif

/* Global state */
static int terminal_initialized = 0;
static int history_enabled = 1;
static int colors_supported = -1; /* -1 = not checked, 0 = no, 1 = yes */

/* Terminal initialization */
int flexon_terminal_init(void) {
    if (terminal_initialized) {
        return 0; /* Already initialized */
    }

#if FLEXON_HAS_READLINE
    /* Readline is already initialized when included */
    terminal_initialized = 1;
    return 0;
#elif FLEXON_HAS_LINENOISE
    /* Linenoise initialization if needed */
    terminal_initialized = 1;
    return 0;
#else
    /* Fallback - no special initialization needed */
    terminal_initialized = 1;
    return 0;
#endif
}

void flexon_terminal_cleanup(void) {
    if (!terminal_initialized) {
        return;
    }

#if FLEXON_HAS_READLINE
    /* Cleanup readline resources */
    #if FLEXON_HAS_GNU_READLINE
        clear_history();
    #endif
#elif FLEXON_HAS_LINENOISE
    /* Cleanup linenoise if needed */
    linenoiseHistoryFree();
#endif

    terminal_initialized = 0;
}

char* flexon_readline(const char* prompt) {
    if (!terminal_initialized) {
        flexon_terminal_init();
    }

#if FLEXON_HAS_READLINE
    /* Use readline/libedit */
    return readline(prompt);
#elif FLEXON_HAS_LINENOISE
    /* Use linenoise */
    return linenoise(prompt);
#else
    /* Fallback implementation */
    static char buffer[1024];
    
    printf("%s", prompt);
    fflush(stdout);
    
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return NULL; /* EOF or error */
    }
    
    /* Remove trailing newline */
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    
    /* Return a copy */
    return strdup(buffer);
#endif
}

void flexon_add_history(const char* line) {
    if (!history_enabled || !line || !*line) {
        return;
    }

#if FLEXON_HAS_READLINE
    add_history(line);
#elif FLEXON_HAS_LINENOISE
    linenoiseHistoryAdd(line);
#else
    /* Fallback - no history support */
    (void)line;
#endif
}

void flexon_clear_history(void) {
#if FLEXON_HAS_GNU_READLINE
    clear_history();
#elif FLEXON_HAS_LIBEDIT
    /* libedit clear_history might not be available */
    #ifdef clear_history
        clear_history();
    #endif
#elif FLEXON_HAS_LINENOISE
    linenoiseHistoryFree();
#else
    /* Fallback - no history to clear */
#endif
}

void flexon_set_history_enabled(int enabled) {
    history_enabled = enabled;
}

int flexon_load_history(const char* filename) {
    if (!filename) {
        return -1;
    }

#if FLEXON_HAS_GNU_READLINE
    return read_history(filename);
#elif FLEXON_HAS_LINENOISE
    return linenoiseHistoryLoad(filename);
#else
    /* Fallback - no history support */
    (void)filename;
    return -1;
#endif
}

int flexon_save_history(const char* filename) {
    if (!filename) {
        return -1;
    }

#if FLEXON_HAS_GNU_READLINE
    return write_history(filename);
#elif FLEXON_HAS_LINENOISE
    return linenoiseHistorySave(filename);
#else
    /* Fallback - no history support */
    (void)filename;
    return -1;
#endif
}

void flexon_set_history_size(int max) {
#if FLEXON_HAS_GNU_READLINE
    stifle_history(max);
#elif FLEXON_HAS_LINENOISE
    linenoiseHistorySetMaxLen(max);
#else
    /* Fallback - no history support */
    (void)max;
#endif
}

int flexon_terminal_supports_colors(void) {
    if (colors_supported != -1) {
        return colors_supported;
    }

#if FLEXON_PLATFORM_WINDOWS
    /* Check for Windows 10+ ANSI support */
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (SetConsoleMode(hOut, dwMode)) {
                colors_supported = 1;
                return 1;
            }
        }
    }
    colors_supported = 0;
    return 0;
#else
    /* Unix-like systems - check if stdout is a terminal */
    if (isatty(STDOUT_FILENO)) {
        /* Check TERM environment variable */
        const char* term = getenv("TERM");
        if (term && (strstr(term, "color") != NULL || 
                     strstr(term, "xterm") != NULL ||
                     strstr(term, "screen") != NULL ||
                     strcmp(term, "linux") == 0)) {
            colors_supported = 1;
            return 1;
        }
    }
    colors_supported = 0;
    return 0;
#endif
}

int flexon_terminal_get_width(void) {
#if FLEXON_PLATFORM_WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return 80; /* Default */
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        return w.ws_col;
    }
    return 80; /* Default */
#endif
}

int flexon_terminal_get_height(void) {
#if FLEXON_PLATFORM_WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
        return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
    return 24; /* Default */
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        return w.ws_row;
    }
    return 24; /* Default */
#endif
}