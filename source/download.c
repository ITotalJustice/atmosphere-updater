#include <stdio.h>
#include <curl/curl.h>
#include <switch.h>
#include <string.h>

#define Megabytes_in_Bytes	1048576
#define API_AGENT           "ITotalJustice"
#define STRING_FILTER       "browser_download_url\":\""

int download_progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow)
{	
    printf("* DOWNLOADING: %.2fMB of %.2fMB *\r", dlnow / Megabytes_in_Bytes, dltotal / Megabytes_in_Bytes);
	
	consoleUpdate(NULL);
	return 0;
}

char* githubAPI(const char *api_url, char *temp_file)
{
    CURL *curl = curl_easy_init();

    if (curl)
    {
        CURLcode res;
        FILE *fp = fopen(temp_file, "wb");

        curl_easy_setopt(curl, CURLOPT_URL, api_url);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, API_AGENT);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);

        if (res == CURLE_OK)
        {
            fp = fopen(temp_file, "r");

            static char ams_zip_url[512];
            char *searchstuff = STRING_FILTER, c;
            int len = strlen(searchstuff) - 1;

            while ((c = fgetc(fp)) != EOF)
            {
                if (c == searchstuff[0])
                {
                    for (int i = 0; c == searchstuff[i]; i++)
                    {
                        c = fgetc(fp);
                        if (i == len)
                        {
                            for (int j = 0; c != '\"'; j++)
                            {
                                ams_zip_url[j] = c;
                                c = fgetc(fp);
                            }
                            fclose(fp);
                            remove(temp_file);
                            return ams_zip_url;
                        }
                    }
                }
            }
        }
    }
    return NULL;
}

int downloadFile(const char *url, const char *output)
{
    CURL *curl = curl_easy_init();

    if (curl)
    {
        printf("\n");

        CURLcode res;
        FILE *fp = fopen(output, "wb");

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, download_progress);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);

        if (res == CURLE_OK)
        {
            printf("\n\ndownload complete!\n\n");
            consoleUpdate(NULL);
            return 0;
        }
    }

    printf("\n\ndownload failed...\n\n");
    consoleUpdate(NULL);
    return 1;
}
