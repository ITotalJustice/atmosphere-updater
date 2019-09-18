#ifndef MENU_H
#define MENU_H

#include "sdl.h"

#define ON                  1
#define OFF                 0
#define CURSOR_LIST_MAX     4
#define small_newline       75
#define newline             110
#define MAX_STRLEN          512

void refreshScreen(void);
void printOptionList(int cursor);
void moreOptionsMenu(int url_location, int menu_tab);
void helpScreen(int url_location, int menu_tab);
void popUpBox(TTF_Font *font, int x, int y, SDL_Colour colour, char *text);

#endif