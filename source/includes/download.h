#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

int githubAPI(const char *api_url, char *temp_file, char *new_url);

int downloadFile(const char *url, const char *output);

#endif