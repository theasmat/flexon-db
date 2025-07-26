#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

static const char *NARROW_LOGO[] = {
    "               $$$$$$                                                 ",
    "           $$$$$$$$$$$$$$                                             ",
    "        $$$$$$$$$$$$$$$$$$$$                                          ",
    "    $$$$$$$$$$$$$$$$$$$$$$$$$$$$                                      ",
    " $$$$$$$$$$$   $$$$$$$$$$$$$$$$$$$$             $$$$$   $$$$          ",
    "$$$$$$$$$      $$$$$$$$$$$$$$$$$$$$$            $$$$$   $$$$          ",
    "$$$$$$$$    $$$$$$$$$$$$$$$$$$$$$$$$            $$$$$   $$$$          ",
    "$$$$$$$$    $$$$$$$$$$$$$$$$$$$$$$$$       $$$  $$$$$   $$$$   $$     ",
    "$$$$$$         $$    $$$$     $$$$$$    $$$$$$$$$$$$$   $$$$$$$$$$$$$ ",
    "$$$$$$$$    $$$$$$    $$    $$$$$$$$   $$$$$   $$$$$$   $$$$$   $$$$$$",
    "$$$$$$$$    $$$$$$$        $$$$$$$$$  $$$$$     $$$$$   $$$$      $$$$",
    "$$$$$$$$    $$$$$$$$$     $$$$$$$$$$  $$$$$     $$$$$   $$$$      $$$$",
    "$$$$$$$$    $$$$$$$$       $$$$$$$$$  $$$$$     $$$$$   $$$$      $$$$",
    "$$$$$$$$    $$$$$$$    $    $$$$$$$$   $$$$$    $$$$$   $$$$$    $$$$$",
    "$$$$$$$$    $$$$$     $$$    $$$$$$$    $$$$$$$$$$$$$   $$$$$$$$$$$$$ ",
    " $$$$$$$    $$$$    $$$$$$    $$$$$       $$$$$  $$$$   $$$$  $$$$    ",
    "    $$$$$$$$$$$$$$$$$$$$$$$$$$$$                                      ",
    "       $$$$$$$$$$$$$$$$$$$$$$                                         ",
    "           $$$$$$$$$$$$$$                                             ",
    "               $$$$$$$                                                ",
};

static const char *TINY_LOGO[] = {
    "          $$$$$$                                  ",
    "      $$$$$$$$$$$$$                               ",
    "   $$$$$$$$$$$$$$$$$$$$                           ",
    "$$$$$$$    $$$$$$$$$$$$$$$        $$$$  $$$       ",
    "$$$$$$   $$$$$$$$$$$$$$$$$        $$$$  $$$       ",
    "$$$$       $   $$$$   $$$$    $$$$$$$$  $$$ $$$$  ",
    "$$$$$$  $$$$$   $   $$$$$$  $$$$ $$$$$  $$$$  $$$$",
    "$$$$$$  $$$$$$     $$$$$$$ $$$$   $$$$  $$$    $$$",
    "$$$$$$  $$$$$$     $$$$$$$ $$$$   $$$$  $$$    $$$",
    "$$$$$$  $$$$$   $   $$$$$$  $$$   $$$$  $$$   $$$$",
    "$$$$$$  $$$$   $$$   $$$$$   $$$$$$$$$  $$$$$$$$  ",
    "   $$$$$$$$$$$$$$$$$$$$                           ",
    "      $$$$$$$$$$$$$                               ",
    "          $$$$$$                                  ",
};

static const char *NANO_LOGO[] =
    {

        "       $$$$                        ",
        "   $$$$$$$$$$$$                    ",
        "$$$$$   $$$$$$$$$$      $$  $$     ",
        "$$$$  $$$$$$$$$$$$      $$  $$     ",
        "$$$    $$  $  $$$$  $$$$$$  $$$$$$$",
        "$$$$  $$$$   $$$$$ $$$  $$  $$  $$$",
        "$$$$  $$$$    $$$$ $$$  $$  $$  $$$",
        "$$$$  $$$ $$$  $$$  $$$$$$  $$$$$$ ",
        "   $$$$$$$$$$$$                    ",
        "       $$$$                        ",
        "                                   ",
};

static const char *minimal_LOGO[] = {
    "   ****             ",
    "*** .*****    * *   ",
    "*.  * * **  *** *** ",
    "** .*.  ** *  * *  *",
    "** .* * .*  *** *** ",
    "   ****             ",
};
static const int NARROW_LOGO_LINES = sizeof(NARROW_LOGO) / sizeof(NARROW_LOGO[0]);
static const int TINY_LOGO_LINES = sizeof(TINY_LOGO) / sizeof(TINY_LOGO[0]);
static const int NANO_LOGO_LINES = sizeof(NANO_LOGO) / sizeof(NANO_LOGO[0]);
static const int MINIMAL_LOGO_LINES = sizeof(minimal_LOGO) / sizeof(minimal_LOGO[0]);

void print_logo(void)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int width = w.ws_col;

    const char **logo_to_use = minimal_LOGO;
    int lines = MINIMAL_LOGO_LINES;

    if (width >= 80)
    {
        logo_to_use = NARROW_LOGO;
        lines = NARROW_LOGO_LINES;
    }
    else if (width >= 60)
    {
        logo_to_use = TINY_LOGO;
        lines = TINY_LOGO_LINES;
    }
    else if (width >= 40)
    {
        logo_to_use = NANO_LOGO;
        lines = NANO_LOGO_LINES;
    }

    for (int i = 0; i < lines; i++)
    {
        int pad = (width - (int)strlen(logo_to_use[i])) / 2;
        if (pad < 0)
            pad = 0;
        for (int j = 0; j < pad; j++)
            putchar(' ');
        puts(logo_to_use[i]);
    }
}