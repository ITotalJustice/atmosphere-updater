#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

char* githubAPI(const char *api_url, char *temp_file);

int downloadFile(const char *url, const char *output);

#endif