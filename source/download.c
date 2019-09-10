#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <curl/curl.h>

#include <switch.h>

#define Megabytes_in_Bytes	1048576
#define API_AGENT           "ITotalJustice"
#define FILTER_STRING       "browser_download_url\":\""

struct MemoryStruct
{
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userdata)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userdata;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if (ptr == NULL) 
  {
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

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
        if (fp)
        {
            struct MemoryStruct chunk;
            chunk.memory = malloc(1);
            chunk.size = 0;

            curl_easy_setopt(curl, CURLOPT_URL, api_url);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, API_AGENT);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

            // write calls
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

            // execute curl, save result
            CURLcode res = curl_easy_perform(curl);

            // write from mem to file
            fwrite(chunk.memory, 1, chunk.size, fp);

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
                    if (c == *FILTER_STRING)
                    {
                        for (int i = 0, len = strlen(FILTER_STRING) - 1; c == FILTER_STRING[i]; i++)
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
        fclose(fp);
    }
    printf("\n\napi get failed...\n\n");
    consoleUpdate(NULL);
    return 1;
}

int downloadFile(const char *url, const char *output)
{
    CURL *curl = curl_easy_init();
    if (curl)
    {
        FILE *fp = fopen(output, "wb");
        if (fp)
        {
            struct MemoryStruct chunk;
            chunk.memory = malloc(1);
            chunk.size = 0;

            printf("\n");
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, API_AGENT);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

            // write calls
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

            // progress calls, still slowish
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, download_progress);

            // execute curl, save result
            CURLcode res = curl_easy_perform(curl);

            // write from mem to file
            fwrite(chunk.memory, 1, chunk.size, fp);

            // clean
            curl_easy_cleanup(curl);
            free(chunk.memory);
            fclose(fp);

            if (res == CURLE_OK)
            {
                printf("\n\ndownload complete!\n\n");
                consoleUpdate(NULL);
                return 0;
            }
        }
        fclose(fp);
    }

    printf("\n\ndownload failed...\n\n");
    consoleUpdate(NULL);
    return 1;
}
