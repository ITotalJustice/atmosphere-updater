#include <unistd.h>
#include <switch.h>

#include "menu.h"

#define APP_VERSION "Atmosphere Updater: 0.4.0"

void refreshScreen(void)
{
    clearRenderer();
    
    drawText(fntMedium, 40, 40, SDL_GetColour(white), APP_VERSION);

    //drawText(fntMedium, 120, 225, SDL_GetColour(white), "Menu Here"); // menu options
    drawText(fntSmall, 950, 675, SDL_GetColour(white), "(A) Select");
    drawText(fntSmall, 1125, 675, SDL_GetColour(white), "(+) Exit");
}

void printOptionList(int cursor)
{
    refreshScreen();

    char *option_list[]      = {    "Full Atmosphere update (recommended)", \
                                    "Update Atmosphere (ignoring .ini files)", \
                                    "Update Hekate (for hekate / kosmos users)", \
                                    "Update app", \
                                    "Reboot (reboot to payload)" };

    char *description_list[] = {    "Update everything for Atmosphere", \
                                    "Update Atmosphere ignoring .ini files (if they exist)", \
                                    "Update hekate with option to also update Atmosphere", \
                                    "Update app and removes old version", \
                                    "Reboots switch (recommended after updating)" };

    SDL_Texture *textureArray[] = { ams_icon, ams_plus_icon, hekate_icon, app_icon, reboot_icon };

    for (int i=0, nl=0; i < (CURSOR_LIST_MAX+1); i++, nl+=NEWLINE)
    {
        if (cursor != i) drawText(fntSmall, 550, (130+nl), SDL_GetColour(white), option_list[i]);
        else
        {
            //drawImageScale(right_arrow, 490, (155+nl), 30, 30);
            drawImage(textureArray[i], 125, 350);
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

void errorBox(char *errorText, SDL_Texture *icon)
{
    popUpBox(fntMedium, 350, 250, SDL_GetColour(white), errorText);
    drawImageScale(icon, 570, 340, 128, 128);
    updateRenderer();

    sleep(3);
}