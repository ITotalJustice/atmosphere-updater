#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <switch.h>

#include "download.h"
#include "unzip.h"
#include "menu.h"
#include "reboot_payload.h"

//#define DEBUG                 // enable for nxlink debug

#define ROOT                    "/"
#define APP_PATH                "/switch/atmosphere-updater/"
#define AMS_OUTPUT              "/switch/atmosphere-updater/ams.zip"
#define HEKATE_OUTPUT           "/switch/atmosphere-updater/hekate.zip"
#define APP_OUTPUT              "/switch/atmosphere-updater/atmosphere-updater.nro"
#define OLD_APP_PATH            "/switch/atmosphere-updater.nro"
#define TEMP_FILE               "/switch/atmosphere-updater/temp"
#define FILTER_STRING           "browser_download_url\":\""

int appInit()
{
    socketInitializeDefault();  // for curl / nxlink
    #ifdef DEBUG
    nxlinkStdio();
    #endif
    plInitialize();             // for shared fonts
    romfsInit();
    sdlInit();
    romfsExit();                // exit romfs after loading sdl as we no longer need it.
    return 0;
}

void appExit()
{
    sdlExit();
    socketExit();
    plExit();
}

int parseSearch(char *phare_string, char *filter, char* new_string)
{
    FILE *fp = fopen(phare_string, "r");
    
    if (fp)
    {
        char c;
        while ((c = fgetc(fp)) != EOF)
        {
            if (c == *FILTER_STRING)
            {
                for (int i = 0, len = strlen(FILTER_STRING) - 1; c == FILTER_STRING[i]; i++)
                {
                    c = fgetc(fp);
                    if (i == len)
                    {
                        for (int j = 0; c != '\"'; j++)
                        {
                            new_string[j] = c;
                            new_string[j+1] = '\0';
                            c = fgetc(fp);
                        }
                        fclose(fp);
                        remove(phare_string);
                        return 0;
                    }
                }
            }
        }
    }

    errorBox("Failed to find parse url!", app_icon);
    fclose(fp);
    return 1;
}

void update_ams_hekate(char *url, char *output, int mode)
{
    if (mode == UP_HEKATE)
    {
        // ask if user wants to install atmosphere as well.

        printOptionList(mode);
        popUpBox(fntMedium, 390, 250, SDL_GetColour(white), "Update AMS and hekate?");
        // highlight box
        drawShape(SDL_GetColour(faint_blue), (380), (410), (175), (65));
        drawShape(SDL_GetColour(faint_blue), (700), (410), (190), (65));
        // option text
        drawText(fntMedium, 410, 425, SDL_GetColour(white), "(B) No");
        drawText(fntMedium, 725, 425, SDL_GetColour(white), "(A) Yes");
        updateRenderer();

        while (1)
        {
            hidScanInput();

            if (hidKeysDown(CONTROLLER_P1_AUTO) & KEY_A)
            {
                update_ams_hekate(AMS_URL, AMS_OUTPUT, UP_AMS);
                break;
            }
            if (hidKeysDown(CONTROLLER_P1_AUTO) & KEY_B) break;
        }
    }

    if (!downloadFile(url, TEMP_FILE, ON))
    {
        char new_url[MAX_STRLEN];
        if (!parseSearch(TEMP_FILE, FILTER_STRING, new_url))
            if (!downloadFile(new_url, output, OFF))
            {
                unzip(output, mode);
            }
    }
}

void update_app()
{
    if (!downloadFile(APP_URL, APP_OUTPUT, OFF))
    {
        remove(OLD_APP_PATH);
        // using errorBox as a message window on this occasion 
        errorBox("Update complete!\nRestart app to take effect", app_icon);
    }
}

int main(int argc, char **argv)
{
    // init stuff
    appInit();
    mkdir(APP_PATH, 0777);
    chdir(ROOT);

    // set the cursor position to 0
    short cursor = 0;

    // TODO: touch

    /*u32 tch = 0;
    touchPosition touch;
    hidTouchRead(&touch, tch);
    if (KEY_TOUCH && touch.px > 530 && touch.px < 1200 && touch.py > 130-20 && touch.py < 130-20+70)*/
    
    // muh loooooop
    while(appletMainLoop())
    {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        // main menu display
        printOptionList(cursor);

        // move cursor down...
        if (kDown & KEY_DOWN)
        {
            if (cursor == CURSOR_LIST_MAX) cursor = 0;
            else cursor++;
        }

        // move cursor up...
        if (kDown & KEY_UP)
        {
            if (cursor == 0) cursor = CURSOR_LIST_MAX;
            else cursor--;
        }

        // select option
        if (kDown & KEY_A)
        {
            switch (cursor)
            {
            case UP_AMS:
                update_ams_hekate(AMS_URL, AMS_OUTPUT, cursor);
                break;

            case UP_AMS_NOINI:
                update_ams_hekate(AMS_URL, AMS_OUTPUT, cursor);
                break;

            case UP_HEKATE:
                update_ams_hekate(HEKATE_URL, HEKATE_OUTPUT, cursor);
                break;

            case UP_APP:
                update_app();
                break;

            case REBOOT_PAYLOAD:
                reboot_payload("/atmosphere/reboot_payload.bin");
                break;
            }
        }
        
        // exit...
        if (kDown & KEY_PLUS) break;

        // display render buffer
        updateRenderer();
    }

    // cleanup then exit
    appExit();
    return 0;
}