#include <unistd.h>
#include <switch.h>

#include "sdl_easy.h"
#include "menu.h"
#include "touch.h"
#include "util.h"

#define APP_VERSION "Atmosphere Updater: 0.5.0"


void refreshScreen()
{
    SDL_ClearRenderer();

    SDL_DrawImage(background, 0, 0);
    
    // app version.
    SDL_DrawText(fntMedium, 40, 40, Colour_White, APP_VERSION);

    // system version.
    SDL_DrawText(fntSmall, 25, 150, Colour_White, getSysVersion());

    // atmosphere version.
    SDL_DrawText(fntSmall, 25, 230, Colour_White, getAmsVersion());

    //SDL_DrawText(fntMedium, 120, 225, Colour_White, "Menu Here"); // menu options
    SDL_DrawButton(fntButton, Font_Button_A, 970, 672, Colour_White);
    SDL_DrawText(fntSmall, 1010, 675, Colour_White, "Select");
    SDL_DrawButton(fntButton, Font_Button_PLUS, 1145, 672, Colour_White);
    SDL_DrawText(fntSmall, 1185, 675, Colour_White, "Exit");
}

void printOptionList(int cursor)
{
    refreshScreen();

    char *option_list[] =
    {
        "Full Atmosphere update (recommended)",
        "Update Atmosphere (ignoring .ini files)",
        "Update Hekate (for hekate / kosmos users)",
        "Update app",
        "Reboot (reboot to payload)"
    };

    char *description_list[] =
    {
        "Update everything for Atmosphere",
        "Update Atmosphere ignoring .ini files (if they exist)",
        "Update hekate with option to also update Atmosphere",
        "Update app and removes old version",
        "Reboots switch (recommended after updating)"
    };

    SDL_Texture *textureArray[] = { ams_icon, ams_plus_icon, hekate_icon, app_icon, reboot_icon };

    for (int i=0, nl=0; i < (CURSOR_LIST_MAX+1); i++, nl+=NEWLINE)
    {
        if (cursor != i)
            SDL_DrawText(fntSmall, 550, FIRST_LINE+nl, Colour_White, option_list[i]);
        else
        {
            // icon for the option selected.
            SDL_DrawImage(textureArray[i], 125, 350);
            // highlight box.
            SDL_DrawShape(Colour_DarkBlue, 530, (FIRST_LINE + nl - HIGHLIGHT_BOX_MIN), 700, HIGHLIGHT_BOX_MAX);
            // option text.
            SDL_DrawText(fntSmall, 550, FIRST_LINE+nl, Colour_JordyBlue, option_list[i]);
            // description.
            SDL_DrawText(fntSmall, 25, 675, Colour_White, description_list[i]);
        }
    }
}

void popUpBox(TTF_Font *font, int x, int y, Colour colour, char *text)
{
    // outline. box
    SDL_DrawShape(Colour_Black, (SCREEN_W/4)-5, (SCREEN_H/4)-5, (SCREEN_W/2)+10, (SCREEN_H/2)+10);
    // popup box.
    SDL_DrawShape(Colour_DarkBlue, SCREEN_W/4, SCREEN_H/4, SCREEN_W/2, SCREEN_H/2);
    // text to draw.
    SDL_DrawText(font, x, y, colour, text);
}

int yesNoBox(int mode, int x, int y, char *question)
{
    printOptionList(mode);
    popUpBox(fntMedium, x, y, Colour_White, question);
    // highlight box.
    SDL_DrawShape(Colour_FaintBlue, 380, 410, 175, 65);
    SDL_DrawShape(Colour_FaintBlue, 700, 410, 190, 65);
    // option text.
    SDL_DrawButton(fntButtonBig, Font_Button_B, 410, 425, Colour_White);
    SDL_DrawText(fntMedium, 455, 425, Colour_White, "No");
    SDL_DrawButton(fntButtonBig, Font_Button_A, 725, 425, Colour_White);
    SDL_DrawText(fntMedium, 770, 425, Colour_White, "Yes");

    SDL_UpdateRenderer();

    int res = 0;
    int touch_lock = OFF;
    touchPosition touch;
    Uint32 tch = 0;
    Uint32 touch_count = hidTouchCount();

    // check if the user is already touching the screen.
    if (touch_count > 0) touch_lock = ON;

    while (1)
    {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        hidTouchRead(&touch, tch);
        touch_count = hidTouchCount();

        if (touch_count == 0) touch_lock = OFF;

        if (touch_count > 0 && touch_lock == OFF)
            res = touch_yes_no_option(touch.px, touch.py);

        if (kDown & KEY_A || res == YES)
            return YES;

        if (kDown & KEY_B || res == NO)
            return NO;
    }
}

void errorBox(int x, int y, char *errorText)
{
    popUpBox(fntMedium, x, y, Colour_White, errorText);
    SDL_DrawImageScale(error_icon, 570, 340, 128, 128);
    SDL_UpdateRenderer();

    sleep(3);
}