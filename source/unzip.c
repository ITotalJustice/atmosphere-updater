#include <stdio.h>
#include <zlib.h>
#include <minizip/unzip.h>
#include <string.h>
#include <dirent.h>
#include <switch.h>

#define WRITEBUFFERSIZE 10000000 // 10MB
#define MAXFILENAME     256

int unzip(const char *output, int mode)
{
    unzFile zfile = unzOpen(output);
    unz_global_info gi;
    unzGetGlobalInfo(zfile, &gi);

    for (int i = 0; i < gi.number_entry; i++)
    {
        char filename_inzip[MAXFILENAME];
        unz_file_info file_info;

        unzOpenCurrentFile(zfile);
        unzGetCurrentFileInfo(zfile, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);

        if (mode == 1 && strstr(filename_inzip, ".ini"))
        {
            // check if file exists, if not, create one anyway
            FILE *f = fopen(filename_inzip, "r");
            if (!f)
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
                printf("creating directory: %s\n", filename_inzip);
                mkdir(filename_inzip, 0777);
            }
        }

        else
        {
            const char *write_filename = filename_inzip;
            FILE *outfile = fopen(write_filename, "wb");
            void *buf = malloc(WRITEBUFFERSIZE);

            printf("writing file: %s\n", write_filename);
            consoleUpdate(NULL);

            for (int j = unzReadCurrentFile(zfile, buf, WRITEBUFFERSIZE); j > 0; j = unzReadCurrentFile(zfile, buf, WRITEBUFFERSIZE))
                fwrite(buf, 1, j, outfile);

            fclose(outfile);
            free(buf);
        }

        jump_to_end: // goto

        unzCloseCurrentFile(zfile);
        unzGoToNextFile(zfile);
        consoleUpdate(NULL);
    }

    unzClose(zfile);
    remove(output);
    printf("\nfinished!\t\tDon't forget to reboot!!!\n");
    consoleUpdate(NULL);

    return 0;
}
