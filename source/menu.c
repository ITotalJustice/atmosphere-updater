#include <switch.h>
#include "menu.h"

#define APP_VERSION "Atmosphere-Updater: 0.4.0"

void refreshScreen(void)
{
    clearRenderer();
    
    drawText(fntMedium, 40, 40, SDL_GetColour(white), APP_VERSION);

    //drawText(fntMedium, 120, 225, SDL_GetColour(white), "Text here"); // menu options
    drawText(fntSmall, 950, 675, SDL_GetColour(white), "(A) Select");
    drawText(fntSmall, 1125, 675, SDL_GetColour(white), "(+) Exit");
}

void printOptionList(int cursor)
{
    refreshScreen();

    char *option_list[]      = { "Full Atmosphere update (recommended)", \
                                "Update Atmosphere, not overwriting .ini files", \
                                "Update Hekate (files and payload)", \
                                "Update app", \
                                "Reboot (reboot to payload)" };

    char *description_list[] = { "Updates everything for Atmosphere", \
                                "Updates ignoring .ini files (if they exist)", \
                                "Updates hekate, reboot payload is replaced with hekate", \
                                "Updates app and removes old version", \
                                "Reboots switch (recommended after updating)" };

    SDL_Texture *textureArray[] = { ams_icon, ams_plus_icon, hekate_icon, app_icon, reboot_icon };

    for (int i=0, nl=0; i < (CURSOR_LIST_MAX+1); i++, nl+=newline)
    {
        if (cursor != i) drawText(fntSmall, 550, (130+nl), SDL_GetColour(white), option_list[i]);
        else
        {
            //drawImageScale(right_arrow, 490, (155+nl), 30, 30);
            drawImageScale(textureArray[i], 125, 350, 256, 256);
            // outline box
            //drawShape(SDL_GetColour(black), (530-4), (130+nl-22), (700+8), (70+5));
            // highlight box
            drawShape(SDL_GetColour(dark_blue), (530), (130+nl-20), (700), (70));
            // option text
            drawText(fntSmall, 550, (130+nl), SDL_GetColour(jordy_blue), option_list[i]);
            // description
            drawText(fntSmall, 25, 675, SDL_GetColour(white), description_list[i]);
        }
    }
}

void popUpBox(TTF_Font *font, int x, int y, SDL_Colour colour, char *text)
{
    // shadow box
    drawShape(SDL_GetColour(black), (SCREEN_W/4-5), (SCREEN_H/4-5), (SCREEN_W/2+10), (SCREEN_H/2+10));
    // popup box
    drawShape(SDL_GetColour(dark_blue), (SCREEN_W/4), (SCREEN_H/4), (SCREEN_W/2), (SCREEN_H/2));
    // text to draw
    drawText(font, x, y, colour, text);
}