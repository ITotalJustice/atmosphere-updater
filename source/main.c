#include <stdio.h>
#include <string.h>
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
#define FILTER_STRING   "browser_download_url\":\""

#define MAX_STRLEN      512

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

void refreshScreen(int cursor)
{
    consoleClear();

    printf("Atmosphere-Updater: v%s.\n\n\n", "0.2.1");

    printf("Press (+) to exit\n\n\n");

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
    DIR *dir = opendir(APP_PATH);
    if (!dir) mkdir(APP_PATH, 0777);
    closedir(dir);

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
            switch (cursor)
            {
            case UP_AMS:
                if (!downloadFile(AMS_URL, TEMP_FILE, ON))
                {
                    char new_url[MAX_STRLEN];
                    if (!pharseSearch(TEMP_FILE, FILTER_STRING, new_url))
                        if (!downloadFile(new_url, AMS_OUTPUT, OFF))
                            unzip(AMS_OUTPUT, UP_AMS_NCONFIG);
                }
                break;

            case UP_AMS_NCONFIG:
                if (!downloadFile(AMS_URL, TEMP_FILE, ON))
                {
                    char new_url[MAX_STRLEN];
                    if (!pharseSearch(TEMP_FILE, FILTER_STRING, new_url))
                        if (!downloadFile(new_url, AMS_OUTPUT, OFF))
                        unzip(AMS_OUTPUT, 1);
                }
                break;

            case UP_APP:
                if (!downloadFile(APP_URL, APP_OUTPUT, OFF))
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