#ifndef PLATFORM_TERMINAL_H
#define PLATFORM_TERMINAL_H

#include "../platform.h"

/* Terminal/Readline abstraction layer */

/* Platform-specific includes */
#if FLEXON_PLATFORM_MACOS && FLEXON_HAVE_LIBEDIT_READLINE
    #include <editline/readline.h>
    #define FLEXON_HAS_READLINE 1
    #define FLEXON_HAS_LIBEDIT 1
#elif FLEXON_PLATFORM_LINUX && FLEXON_HAVE_GNU_READLINE
    #include <readline/readline.h>
    #include <readline/history.h>
    #define FLEXON_HAS_READLINE 1
    #define FLEXON_HAS_GNU_READLINE 1
#elif (FLEXON_PLATFORM_WINDOWS || FLEXON_PLATFORM_MOBILE) && defined(FLEXON_HAS_LINENOISE)
    #include "linenoise.h"
    #define FLEXON_HAS_LINENOISE 1
#endif

/* Fallback definitions */
#ifndef FLEXON_HAS_READLINE
#define FLEXON_HAS_READLINE 0
#endif

#ifndef FLEXON_HAS_LINENOISE
#define FLEXON_HAS_LINENOISE 0
#endif

#ifndef FLEXON_HAS_LIBEDIT
#define FLEXON_HAS_LIBEDIT 0
#endif

#ifndef FLEXON_HAS_GNU_READLINE
#define FLEXON_HAS_GNU_READLINE 0
#endif

/* Unified terminal interface */

/**
 * Initialize the terminal subsystem
 * @return 0 on success, -1 on error
 */
int flexon_terminal_init(void);

/**
 * Cleanup the terminal subsystem
 */
void flexon_terminal_cleanup(void);

/**
 * Read a line from the terminal with prompt
 * @param prompt The prompt to display
 * @return Allocated string with user input, or NULL on EOF/error
 *         Caller must free() the returned string
 */
char* flexon_readline(const char* prompt);

/**
 * Add a line to the history
 * @param line The line to add to history
 */
void flexon_add_history(const char* line);

/**
 * Clear the command history
 */
void flexon_clear_history(void);

/**
 * Enable/disable history functionality
 * @param enabled 1 to enable, 0 to disable
 */
void flexon_set_history_enabled(int enabled);

/**
 * Load history from file
 * @param filename Path to history file
 * @return 0 on success, -1 on error
 */
int flexon_load_history(const char* filename);

/**
 * Save history to file
 * @param filename Path to history file
 * @return 0 on success, -1 on error
 */
int flexon_save_history(const char* filename);

/**
 * Set the maximum number of history entries
 * @param max Maximum number of entries (0 for unlimited)
 */
void flexon_set_history_size(int max);

/**
 * Check if terminal supports colors
 * @return 1 if colors are supported, 0 otherwise
 */
int flexon_terminal_supports_colors(void);

/**
 * Get terminal width in characters
 * @return Terminal width, or 80 if unknown
 */
int flexon_terminal_get_width(void);

/**
 * Get terminal height in characters
 * @return Terminal height, or 24 if unknown
 */
int flexon_terminal_get_height(void);

#endif /* PLATFORM_TERMINAL_H */