#ifndef _MENU_H_
#define _MENU_H_

#include "sdl.h"

#define ON                  1
#define OFF                 0
#define CURSOR_LIST_MAX     4
#define NEWLINE             110
#define MAX_STRLEN          512

void refreshScreen(void);
void printOptionList(int cursor);
void helpScreen(int url_location, int menu_tab);
void popUpBox(TTF_Font *font, int x, int y, SDL_Colour colour, char *text);
void errorBox(char *errorText, SDL_Texture *icon);

#endif