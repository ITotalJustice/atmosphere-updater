#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <switch.h>

#include "util.h"
#include "menu.h"
#include "unzip.h"
#include "download.h"
#include "reboot_payload.h"

//#define DEBUG                 // enable for nxlink debug

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

    Result rc;

    if (R_FAILED(rc = setsysInitialize()))  // For Horizon OS Version
        printf("setsysInitialize() failed: 0x%x.\n\n", rc);

    if (R_FAILED(rc = splInitialize()))     // For Atmosphere version
        printf("splInitialize() failed: 0x%x.\n\n", rc);

    return 0;
}

void appExit()
{
    sdlExit();
    socketExit();
    plExit();
    splExit();
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