#include <stdio.h>
#include <string.h>
#include <switch.h>

#include "util.h"
#include "menu.h"
#include "unzip.h"
#include "download.h"
#include "reboot_payload.h"

#define TEMP_FILE               "/switch/atmosphere-updater/temp"
#define FILTER_STRING           "browser_download_url\":\""


void copyFile(char *src, char *dest)
{
    FILE *srcfile = fopen(src, "r");
    FILE *newfile = fopen(dest, "wb");

    if (srcfile && newfile)
    {
        char buffer[10000]; // 10kb per write, which is fast
        size_t bytes;

        while (0 < (bytes = fread(buffer, 1, sizeof(buffer), srcfile)))
        {
            fwrite(buffer, 1, bytes, newfile);
        }
    }
    fclose(srcfile);
    fclose(newfile);
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

    errorBox(350, 250, "Failed to find parse url!");
    fclose(fp);
    return 1;
}

int update_ams_hekate(char *url, char *output, int mode)
{
    if (mode == UP_HEKATE)
    {
        // ask if user wants to install atmosphere as well.
        yesNoBox(mode, 390, 250, "Update AMS and hekate?");

        while (1)
        {
            hidScanInput();

            if (hidKeysDown(CONTROLLER_P1_AUTO) & KEY_A)
            {
                // ask if user wants to overwite the atmosphere ini files.
                yesNoBox(mode, 355, 250, "Overwite Atmosphere ini files?");

                while (1)
                {
                    hidScanInput();

                    if (hidKeysDown(CONTROLLER_P1_AUTO) & KEY_A)
                    {
                        if (!update_ams_hekate(AMS_URL, AMS_OUTPUT, UP_AMS))
                            rename("/atmosphere/reboot_payload.bin", "/bootloader/payloads/fusee-primary.bin");
                        break;
                    }
                    if (hidKeysDown(CONTROLLER_P1_AUTO) & KEY_B)
                    {
                        if (!update_ams_hekate(AMS_URL, AMS_OUTPUT, UP_AMS_NOINI))
                            rename("/atmosphere/reboot_payload.bin", "/bootloader/payloads/fusee-primary.bin");
                        break;
                    }
                }
                break;
            }
            if (hidKeysDown(CONTROLLER_P1_AUTO) & KEY_B) break;
        }
    }

    if (!downloadFile(url, TEMP_FILE, ON))
    {
        char new_url[MAX_STRLEN];
        if (!parseSearch(TEMP_FILE, FILTER_STRING, new_url))
        {
            if (!downloadFile(new_url, output, OFF))
            {
                unzip(output, mode);

                // check if an update.bin is present, remove if so.
                if (mode == UP_HEKATE)
                {
                    copyFile("/atmosphere/reboot_payload.bin", "/bootloader/update.bin");
                }
                return 0;
            }
            return 1;
        }
        return 1;
    }
    return 1;
}

void update_app()
{
    if (!downloadFile(APP_URL, APP_OUTPUT, OFF))
    {
        remove(OLD_APP_PATH);
        // using errorBox as a message window on this occasion 
        errorBox(400, 250, "      Update complete!\nRestart app to take effect");
    }
}