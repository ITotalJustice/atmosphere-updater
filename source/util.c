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



char *GetFirmwareVersion() {
	Result ret = 0;
	SetSysFirmwareVersion ver;

	if (R_FAILED(ret = setsysGetFirmwareVersion(&ver))) 
    {
		printf("GetFirmwareVersion() failed: 0x%x.\n\n", ret);
		return NULL;
	}

	static char buf[10];
	snprintf(buf, 19, "%u.%u.%u-%u%u", ver.major, ver.minor, ver.micro, ver.revision_major, ver.revision_minor);
    
	return buf;
}

char *GetAtmosphereVersion(void) {
	Result ret = 0;
	u64 ver;
    u64 verhash;
    SplConfigItem SplConfigItem_ExosphereVersion = (SplConfigItem)65000;
    SplConfigItem SplConfigItem_ExosphereVerHash = (SplConfigItem)65003;

	if (R_FAILED(ret = splGetConfig(SplConfigItem_ExosphereVersion, &ver))) 
    {
		printf("SplConfigItem_ExosphereVersion() failed: 0x%x.\n\n", ret);
		return NULL;
	}

    if (R_FAILED(ret = splGetConfig(SplConfigItem_ExosphereVerHash, &verhash))) 
    {
		printf("SplConfigItem_ExosphereVerHash() failed: 0x%x.\n\n", ret);
		return NULL;
	}

    static char verbuf[20];
	snprintf(verbuf, 20, "-%lx", verhash);
    char vetarget[20];
    *vetarget = '\0';
    strncat(vetarget, verbuf, 8);

	static char buf[100];
	snprintf(buf, 1000, "%lu.%lu.%lu%s", (ver >> 32) & 0xFF,  (ver >> 24) & 0xFF, (ver >> 16) & 0xFF, vetarget);

	return buf;
}

void copyFile(char *src, char *dest)
{
    FILE *srcfile = fopen(src, "rb");
    FILE *newfile = fopen(dest, "wb");

    if (srcfile && newfile)
    {
        char buffer[10000]; // 10kb per write, which is fast
        size_t bytes;       // size of the file to write (10kb or filesize max)

        while (0 < (bytes = fread(buffer, 1, sizeof(buffer), srcfile)))
        {
            fwrite(buffer, 1, bytes, newfile);
        }
    }
    fclose(srcfile);
    fclose(newfile);
}

int parseSearch(char *parse_string, char *filter, char* new_string)
{
    FILE *fp = fopen(parse_string, "r");
    
    if (fp)
    {
        char c;
        while ((c = fgetc(fp)) != EOF)
        {
            if (c == *filter)
            {
                for (int i = 0, len = strlen(filter) - 1; c == filter[i]; i++)
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
                        remove(parse_string);
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
    // download new nro as a tempfile.
    if (!downloadFile(APP_URL, TEMP_FILE, OFF))
    {
        // remove current nro file.
        remove(APP_OUTPUT);
        // remove nro from /switch/.
        remove(OLD_APP_PATH);
        // rename the downloaded temp_file with the correct nro name.
        rename(TEMP_FILE, APP_OUTPUT);
        // using errorBox as a message window on this occasion. 
        errorBox(400, 250, "      Update complete!\nRestart app to take effect");
    }
}
