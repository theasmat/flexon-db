#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "logo.h"

static void print_line(void)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    for (int i = 0; i < w.ws_col; i++)
        putchar('=');
    putchar('\n');
}

void print_welcome_message(void)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int width = w.ws_col;

    const char *msg = "🚀 Welcome to FlexonDB Interactive Shell";

    print_logo(); // From logo module
    printf("\n");

    print_line();

    int padding = (width - (int)strlen(msg)) / 2;
    if (padding < 0)
        padding = 0;

    for (int i = 0; i < padding; i++)
        putchar(' ');

    // Bold print
    printf("\033[1m%s\033[0m\n", msg);

    print_line();
    printf("\n");
}