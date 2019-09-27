#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <switch.h>

#include "util.h"
#include "menu.h"
#include "unzip.h"
#include "download.h"
#include "reboot_payload.h"

//#define DEBUG                                              // enable for nxlink debug

int appInit()
{
    Result rc;

    if (R_FAILED(rc = socketInitializeDefault()))           // for curl / nxlink.
        printf("socketInitializeDefault() failed: 0x%x.\n\n", rc);

    #ifdef DEBUG
    if (R_FAILED(rc = nxlinkStdio()))                       // redirect all printout to console window.
        printf("nxlinkStdio() failed: 0x%x.\n\n", rc)
    #endif

    if (R_FAILED(rc = setsysInitialize()))                  // for system version
        printf("setsysInitialize() failed: 0x%x.\n\n", rc);

    if (R_FAILED(rc = splInitialize()))                     // for atmosphere version
        printf("splInitialize() failed: 0x%x.\n\n", rc);

    if (R_FAILED(rc = plInitialize()))                      // for shared fonts.
        printf("plInitialize() failed: 0x%x.\n\n", rc);

    if (R_FAILED(rc = romfsInit()))                         // load textures from app.
        printf("romfsInit() failed: 0x%x.\n\n", rc);

    sdlInit();                                              // int all of sdl and start loading textures.

    romfsExit();                                            // exit romfs after loading sdl as we no longer need it.

    return 0;
}

void appExit()
{
    sdlExit();
    socketExit();
    plExit();
    splExit();
    setsysExit();
}

int main(int argc, char **argv)
{
    // init stuff.
    appInit();
    mkdir(APP_PATH, 0777);
    chdir(ROOT);

    // write sys / ams version to char*.
    writeSysVersion();
    writeAmsVersion();

    // set the cursor position to 0.
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