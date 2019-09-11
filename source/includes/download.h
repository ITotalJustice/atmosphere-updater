#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

#define AMS_URL "https://api.github.com/repos/Atmosphere-NX/Atmosphere/releases/latest"
#define APP_URL "https://github.com/ITotalJustice/atmosphere-updater/releases/latest/download/atmosphere-updater.nro"

#define ON      1
#define OFF     2

int downloadFile(const char *url, const char *output, int api_mode);

#endif