#include <stdio.h>
#include <minizip/unzip.h>
#include <string.h>
#include <dirent.h>
#include <switch.h>

#include "unzip.h"
#include "menu.h"

#define WRITEBUFFERSIZE 500000 // 500KB
#define MAXFILENAME     256

int unzip(const char *output, int mode)
{
    unzFile zfile = unzOpen(output);
    unz_global_info gi;
    unzGetGlobalInfo(zfile, &gi);

    for (int i = 0; i < gi.number_entry; i++)
    {
        printOptionList(mode);
        popUpBox(fntSmall, 350, 250, SDL_GetColour(white), "Unziping...");

        char filename_inzip[MAXFILENAME];
        unz_file_info file_info;

        unzOpenCurrentFile(zfile);
        unzGetCurrentFileInfo(zfile, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);

        if (mode == UP_AMS_NOINI && strstr(filename_inzip, ".ini"))
        {
            // check if file exists, if not, create one anyway
            FILE *f = fopen(filename_inzip, "r");
            if (f)
            {
                fclose(f);
                goto jump_to_end;
            }
            fclose(f);
        }

        // check if the string ends with a /, if so, then its a directory.
        if ((filename_inzip[strlen(filename_inzip) - 1]) == '/')
        {
            // check if directory exists
            DIR *dir = opendir(filename_inzip);
            if (dir) closedir(dir);
            else
            {
                drawText(fntSmall, 350, 350, SDL_GetColour(white), filename_inzip);
                mkdir(filename_inzip, 0777);
            }
        }

        else
        {
            const char *write_filename = filename_inzip;
            void *buf = malloc(WRITEBUFFERSIZE);

            FILE *outfile;
            if (mode == UP_HEKATE && strstr(filename_inzip, ".bin")) outfile = fopen("/atmosphere/reboot_payload.bin", "wb");
            else outfile = fopen(write_filename, "wb");

            drawText(fntSmall, 350, 350, SDL_GetColour(white), write_filename);

            for (int j = unzReadCurrentFile(zfile, buf, WRITEBUFFERSIZE); j > 0; j = unzReadCurrentFile(zfile, buf, WRITEBUFFERSIZE))
                fwrite(buf, 1, j, outfile);

            fclose(outfile);
            free(buf);
        }

        updateRenderer();

        jump_to_end: // goto
        unzCloseCurrentFile(zfile);
        unzGoToNextFile(zfile);
    }

    unzClose(zfile);
    remove(output);
    return 0;
}
