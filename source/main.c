#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <switch.h>

#include "includes/download.h"
#include "includes/unzip.h"
#include "includes/reboot_payload.h"

#define ROOT            "/"
#define APP_PATH        "/switch/atmosphere-updater/"
#define AMS_OUTPUT      "/switch/atmosphere-updater/ams.zip"
#define APP_OUTPUT      "/switch/atmosphere-updater/atmosphere-updater.nro"
#define OLD_APP_PATH    "/switch/atmosphere-updater.nro"
#define TEMP_FILE       "/switch/atmosphere-updater/temp"
#define FILTER_STRING   "browser_download_url\":\""

#define APP_VERSION     "0.3.0"
#define MAX_STRLEN      512
#define CURSOR_LIST_MAX 3

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

    printf("Atmosphere-Updater: v%s.\n\n\n", APP_VERSION);

    printf("Press (+) to exit\n\n\n");

    char *option_list[] = {"= Full Atmosphere Update (recommended)", "= Update Atmosphere, not overwriting \".ini\" files", \
    "= update this app", "= reboot switch (reboot to payload)"};
    for (int i = 0; i < (CURSOR_LIST_MAX + 1); i++)
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

    //SocketInitConfig socket;

    // make paths
    DIR *dir = opendir(APP_PATH);
    if (!dir) mkdir(APP_PATH, 0777);
    closedir(dir);

    // set the cursor position to 0
    short cursor = 0;
    refreshScreen(cursor);

    // muh loooooop
    while(appletMainLoop())
    {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        // move cursor down...
        if (kDown & KEY_DOWN)
        {
            if (cursor == CURSOR_LIST_MAX) cursor = 0;
            else cursor++;
            refreshScreen(cursor);
        }

        // move cursor up...
        if (kDown & KEY_UP)
        {
            if (cursor == 0) cursor = CURSOR_LIST_MAX;
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

            case REBOOT_PAYLOAD:
                if (!reboot_payload("/atmosphere/reboot_payload.bin"))
                    printf("Failed to reboot to payload...\n");
                break;
            }
        }
        
        // exit...
        if (kDown & KEY_PLUS) break;
    }

    // cleanup then exit
    socketExit();
    consoleExit(NULL);
    return 0;
}