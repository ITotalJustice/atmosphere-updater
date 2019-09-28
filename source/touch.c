#include <switch.h>

#include "menu.h"
#include "util.h"

#define TOUCH_MIN 20
#define TOUCH_MAX 90

int touch_cursor(int x, int y)
{
    for (int cursor = 0, nl=0; cursor < (CURSOR_LIST_MAX+1); cursor++, nl+=NEWLINE)
        if (y > FIRST_LINE+nl-TOUCH_MIN && y < FIRST_LINE+TOUCH_MAX+nl)
            return cursor;
}

int touch_yes_no_option(int x, int y)
{
    if (x > 380 && x < 555 && y > 410 && y < 475)
        return NO;

    if (x > 700 && x < 890 && y > 410 && y < 475)
        return YES;

    return 1;
}