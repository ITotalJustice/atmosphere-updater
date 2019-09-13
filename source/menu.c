#include <switch.h>
#include "menu.h"


void refreshScreen(void)
{
    clearRenderer();
    
    drawText(fntMedium, 40, 40, SDL_GetColour(white), "Atmosphere-Updater: 0.3.0");

    //drawText(fntMedium, 120, 225, SDL_GetColour(white), "Text here"); // menu options
    drawText(fntSmall, 950, 675, SDL_GetColour(white), "(A) Select");
    drawText(fntSmall, 1125, 675, SDL_GetColour(white), "(+) Exit");
}

void printOptionList(int cursor)
{
    refreshScreen();

    char *option_list[]      = { "Full Atmosphere update (recommended)", \
                                "Update Atmosphere, not overwriting .ini files", \
                                "Update app", \
                                "Reboot (reboot to payload)" };

    char *description_list[] = { "Updates everything for Atmosphere", \
                                "Updates ignoring .ini files (if they exist)", \
                                "Updates app and removes old version",
                                "Reboots switch (recommended after updating)" };

    SDL_Texture *textureArray[] = { ams_icon, ams_plus_icon, app_icon, reboot_icon };

    for (int i=0, nl=0; i < (CURSOR_LIST_MAX+1); i++, nl+=newline)
    {
        if (cursor != i) drawText(fntSmall, 550, (160+nl), SDL_GetColour(white), option_list[i]);
        else
        {
            //drawImageScale(right_arrow, 490, (155+nl), 30, 30);
            drawImage(textureArray[i], 125, 350);
            drawText(fntSmall, 550, (160+nl), SDL_GetColour(jordy_blue), option_list[i]);
            drawText(fntSmall, 25, 675, SDL_GetColour(white), description_list[i]);
        }
    }
}

void popUpBox(TTF_Font *font, int x, int y, SDL_Colour colour, char *text)
{
    drawShape(SDL_GetColour(dark_grey), (SCREEN_W/4), (SCREEN_H/4), (SCREEN_W/2), (SCREEN_H/2));
    drawText(font, x, y, colour, text);
}