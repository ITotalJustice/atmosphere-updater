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
#define OLD_APP_PATH    "/switch/atmosphere-updater.nro"
#define TEMP_FILE       "/switch/atmosphere-updater/temp"

#define UP_AMS          0
#define UP_AMS_NCONFIG  1
#define UP_APP          2
#define MAX_STRLEN      512

void refreshScreen(int cursor)
{
    consoleClear();

    printf("Atmosphere-Updater: v%.2f.\n\n\n", 0.20);

    char *option_list[] = {"= Full Atmosphere Update (recommended)", "= Update Atmosphere, not overwriting \".ini\" files", "= update this app"};
    for (int i = 0; i < 3; i++)
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

    short cursor = 0, cursor_max = 2;
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
            char new_url[MAX_STRLEN];
            switch (cursor)
            {
            case UP_AMS:
                if ((githubAPI(AMS_URL, TEMP_FILE, new_url)) == 0)
                    if (!downloadFile(new_url, AMS_OUTPUT))
                        unzip(AMS_OUTPUT, 0);
                break;

            case UP_AMS_NCONFIG:
                if ((githubAPI(AMS_URL, TEMP_FILE, new_url)) == 0)
                    if (!downloadFile(new_url, AMS_OUTPUT))
                        unzip(AMS_OUTPUT, 1);
                break;

            case UP_APP:
                if (downloadFile(APP_URL, APP_OUTPUT) == 0)
                {
                    FILE *f = fopen(OLD_APP_PATH, "r");
                    if (f) 
                    {
                        fclose(f);
                        remove(OLD_APP_PATH);
                    }
                    else fclose(f);
                }
                break;
            }
        }
        
        if (kDown & KEY_PLUS) break;
    }

    // cleanup then exit
    socketExit();
    consoleExit(NULL);
    return 0;
}