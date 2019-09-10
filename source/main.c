#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <switch.h>

#include "includes/download.h"
#include "includes/unzip.h"

#define AMS_URL         "https://api.github.com/repos/Atmosphere-NX/Atmosphere/releases/latest"
#define APP_URL         "https://github.com/ITotalJustice/atmosphere-updater/releases/latest/download/atmosphere-updater.nro"

#define ROOT            "/"
#define APP_PATH        "/switch/atmosphere-updater/"
#define AMS_OUTPUT      "/switch/atmosphere-updater/ams.zip"
#define APP_OUTPUT      "/switch/atmosphere-updater/atmosphere-updater.nro"
#define TEMP_FILE       "/switch/atmosphere-updater/temp"

#define UP_AMS          0
#define UP_APP          1
#define MAX_BUFFER      512

void refreshScreen(int cursor)
{
    consoleClear();

    char *option_list[] = {"= Update AMS", "= update this app"};

    printf("Atmosphere-Updater: v%.2f.\n\n\n", 0.1);

    for (int i = 0; i < 2; i++)
    {
        if (cursor != i) printf("[ ] %s\n\n", option_list[i]);
        else printf("[X] %s\n\n", option_list[i]);
    }

    consoleUpdate(NULL);
}

int main(int argc, char **argv)
{
    // init stuff
    socketInitializeDefault();
    consoleInit(NULL);
    chdir(ROOT);

    // make paths
    mkdir(APP_PATH, 0777);

    short cursor = 0, cursor_max = 1;
    refreshScreen(cursor);

    // muh loooooop
    while(appletMainLoop())
    {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        // move cursor down...
        if (kDown & KEY_DOWN)
        {
            if (cursor == cursor_max) cursor = 0;
            else cursor++;
            refreshScreen(cursor);
        }

        // move cursor up...
        if (kDown & KEY_UP)
        {
            if (cursor == 0) cursor = cursor_max;
            else cursor--;
            refreshScreen(cursor);
        }

        if (kDown & KEY_A)
        {
            if (cursor == UP_AMS)
            {
                char new_url[MAX_BUFFER];
                if ((githubAPI(AMS_URL, TEMP_FILE, new_url)) == 0)
                    if (!downloadFile(new_url, AMS_OUTPUT))
                        unzip(AMS_OUTPUT);
            }

            else if (cursor == UP_APP)
                downloadFile(APP_URL, APP_OUTPUT);
        }
        
        if (kDown & KEY_PLUS) break;
    }

    // cleanup then exit
    socketExit();
    consoleExit(NULL);
    return 0;
}