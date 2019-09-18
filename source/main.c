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

//#define DEBUG           // enable for nxlink debug

#define ROOT            "/"
#define APP_PATH        "/switch/atmosphere-updater/"
#define AMS_OUTPUT      "/switch/atmosphere-updater/ams.zip"
#define HEKATE_OUTPUT   "/switch/atmosphere-updater/hekate.zip"
#define APP_OUTPUT      "/switch/atmosphere-updater/atmosphere-updater.nro"
#define OLD_APP_PATH    "/switch/atmosphere-updater.nro"
#define TEMP_FILE       "/switch/atmosphere-updater/temp"
#define FILTER_STRING   "browser_download_url\":\""

int appInit()
{
    socketInitializeDefault();
    #ifdef DEBUG
    nxlinkStdio();
    #endif
    plInitialize();
    romfsInit();
    sdlInit();
    return 0;
}

void appExit()
{
    sdlExit();
    romfsExit();
    socketExit();
    plExit();
}

int pharseSearch(char *phare_string, char *filter, char* new_string)
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
    fclose(fp);
    return 1;
}

int main(int argc, char **argv)
{
    // init stuff
    appInit();
    mkdir(APP_PATH, 0777);
    chdir(ROOT);

    // set the cursor position to 0
    short cursor        = 0;

    // bools
    bool app_update     = OFF;
    bool old_path       = OFF;

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

        if (kDown & KEY_A)
        {
            switch (cursor)
            {
            case UP_AMS:
                if (!downloadFile(AMS_URL, TEMP_FILE, ON))
                {
                    char new_url[MAX_STRLEN];
                    if (!pharseSearch(TEMP_FILE, FILTER_STRING, new_url))
                        if (!downloadFile(new_url, AMS_OUTPUT, OFF))
                            unzip(AMS_OUTPUT, UP_AMS);
                }
                break;

            case UP_AMS_NCONFIG:
                if (!downloadFile(AMS_URL, TEMP_FILE, ON))
                {
                    char new_url[MAX_STRLEN];
                    if (!pharseSearch(TEMP_FILE, FILTER_STRING, new_url))
                        if (!downloadFile(new_url, AMS_OUTPUT, OFF))
                        unzip(AMS_OUTPUT, UP_AMS_NCONFIG);
                }
                break;

            case UP_HEKATE:
                if (!downloadFile(HEKATE_URL, TEMP_FILE, ON))
                {
                    char new_url[MAX_STRLEN];
                    if (!pharseSearch(TEMP_FILE, FILTER_STRING, new_url))
                        if (!downloadFile(new_url, HEKATE_OUTPUT, OFF))
                            unzip(HEKATE_OUTPUT, UP_HEKATE);
                }
                break;

            case UP_APP:
                if (!downloadFile(APP_URL, TEMP_FILE, OFF))
                {
                    FILE *f1 = fopen(APP_OUTPUT, "r");
                    FILE *f2 = fopen(OLD_APP_PATH, "r");
                    if (f1) 
                    {
                        app_update = ON;
                        fclose(f1);
                    }
                    if (f2)
                    {
                        app_update = ON;
                        old_path = ON;
                        fclose(f2);
                    }
                    drawShape(SDL_GetColour(dark_grey), (SCREEN_W/4), (SCREEN_H/4), (SCREEN_W/2), (SCREEN_H/2));
                    drawImageScale(app_icon, 570, 340, 128, 128);
                    drawText(fntMedium, 350, 250, SDL_GetColour(white), "Update complete! Closing app...");
                    updateRenderer();

                    sleep(3);
                    goto jump_exit;
                }
                break;

            case REBOOT_PAYLOAD:
                if (!reboot_payload("/atmosphere/reboot_payload.bin"))
                {
                    drawShape(SDL_GetColour(dark_grey), (SCREEN_W/4), (SCREEN_H/4), (SCREEN_W/2), (SCREEN_H/2));
                    drawImageScale(reboot_icon, 570, 340, 128, 128);
                    drawText(fntMedium, 350, 250, SDL_GetColour(white), "Failed to reboot to payload...");
                    updateRenderer();

                    sleep(3);
                }
                break;
            }
        }
        
        // exit...
        if (kDown & KEY_PLUS) break;

        // display render buffer
        updateRenderer();
    }

    jump_exit: //goto

    // cleanup then exit
    appExit();

    // runs if the app was updated
    if (app_update)
    {
        if (old_path) remove(OLD_APP_PATH);
        else remove(APP_OUTPUT);
        rename(TEMP_FILE, APP_OUTPUT);
    }

    return 0;
}