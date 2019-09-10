#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <curl/curl.h>

#include <switch.h>

#define Megabytes_in_Bytes	1048576
#define API_AGENT           "ITotalJustice"
#define STRING_FILTER       "browser_download_url\":\""
                    
int download_progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow)
{
    if (dltotal <= 0.0) return 0;

    printf("* DOWNLOADING: %.2fMB of %.2fMB *\r", dlnow / Megabytes_in_Bytes, dltotal / Megabytes_in_Bytes);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    int counter = round(tv.tv_usec / 100000);

    if (counter == 0 || counter == 2 || counter == 4 || counter == 6 || counter == 8)
    {
        consoleUpdate(NULL);
    }
    fflush(stdout);
	return 0;
}

int githubAPI(const char *api_url, char *temp_file, char *new_url)
{
    CURL *curl = curl_easy_init();

    if (curl)
    {
        FILE *fp = fopen(temp_file, "wb");

        curl_easy_setopt(curl, CURLOPT_URL, api_url);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, API_AGENT);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        // execute curl
        CURLcode res = curl_easy_perform(curl);

        //clean
        curl_easy_cleanup(curl);
        fclose(fp);

        // slow way of reading throught the file to find a string.
        // should use a json lib to read through the file much faster.
        if (res == CURLE_OK)
        {
            fp = fopen(temp_file, "r");
            char c;

            while ((c = fgetc(fp)) != EOF)
            {
                if (c == *STRING_FILTER)
                {
                    for (int i = 0, len = strlen(STRING_FILTER) - 1; c == STRING_FILTER[i]; i++)
                    {
                        c = fgetc(fp);
                        if (i == len)
                        {
                            for (int j = 0; c != '\"'; j++)
                            {
                                new_url[j] = c;
                                new_url[j+1] = '\0';
                                c = fgetc(fp);
                            }
                            fclose(fp);
                            remove(temp_file);
                            return 0;
                        }
                    }
                }
            }
        }
    }
    return 1;
}

int downloadFile(const char *url, const char *output)
{
    CURL *curl = curl_easy_init();

    if (curl)
    {
        printf("\n");
        FILE *fp = fopen(output, "wb");

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, download_progress);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        // execute curl
        CURLcode res = curl_easy_perform(curl);

        // clean
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
