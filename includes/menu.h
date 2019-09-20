#ifndef _MENU_H_
#define _MENU_H_

#include "sdl.h"

#define ON                  1
#define OFF                 0
#define CURSOR_LIST_MAX     4
#define NEWLINE             110
#define MAX_STRLEN          512

void refreshScreen(void);                                                   // clear screen, display background with title
void printOptionList(int cursor);                                           // print list of options
void popUpBox(TTF_Font *font, int x, int y, SDL_Colour colour, char *text); // pop-up box in center of screen
void yesNoBox(int mode, int x, int y, char *question);                      // box with yes or no option
void errorBox(int x, int y, char *errorText);                               // used for displaying error messages

#endif