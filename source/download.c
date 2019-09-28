#include <stdio.h>
#include <time.h>
#include <curl/curl.h>
#include <switch.h>

#include "download.h"
#include "menu.h"
#include "util.h"           // for ON / OFF defines.

#define API_AGENT           "ITotalJustice"
#define DOWNLOAD_BAR_MAX    500

struct MemoryStruct
{
  char *memory;
  size_t size;
  int mode;
};

static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userdata)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userdata;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);

  if (ptr == NULL)
  {
      errorBox(350, 250, "Failed to realloc mem");
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
    // if file empty or download hasn't started, exit.
    if (dltotal <= 0.0) return 0;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    int counter = tv.tv_usec / 100000;

    // update progress bar every so often.
    if (counter == 0 || counter == 2 || counter == 4 || counter == 6 || counter == 8)
    {
        printOptionList(0);
        popUpBox(fntSmall, 350, 250, SDL_GetColour(white), "Downloading...");
        // bar max size
        drawShape(SDL_GetColour(white), 380, 380, DOWNLOAD_BAR_MAX, 30);
        // progress bar being filled
        drawShape(SDL_GetColour(faint_blue), 380, 380, (dlnow / dltotal) * DOWNLOAD_BAR_MAX, 30);

        updateRenderer();
    }
	return 0;
}

int downloadFile(const char *url, const char *output, int api_mode)
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

            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, API_AGENT);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

            // write calls
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

            // progress calls, ssl still slow
            if (api_mode == OFF)
            {
                curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
                curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, download_progress);
            }

            // execute curl, save result
            CURLcode res = curl_easy_perform(curl);

            // write from mem to file
            fwrite(chunk.memory, 1, chunk.size, fp);

            // clean
            curl_easy_cleanup(curl);
            free(chunk.memory);
            fclose(fp);

            if (res == CURLE_OK) return 0;
        }
        fclose(fp);
    }

    errorBox(350, 250, "Download failed...");
    return 1;
}
