#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

#define ON      1
#define OFF     2

int downloadFile(const char *url, const char *output, int api_mode);

#endif