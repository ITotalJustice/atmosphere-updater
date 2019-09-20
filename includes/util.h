#ifndef _UTIL_H_
#define _UTIL_H_

#define ROOT                    "/"
#define APP_PATH                "/switch/atmosphere-updater/"
#define AMS_OUTPUT              "/switch/atmosphere-updater/ams.zip"
#define HEKATE_OUTPUT           "/switch/atmosphere-updater/hekate.zip"
#define APP_OUTPUT              "/switch/atmosphere-updater/atmosphere-updater.nro"
#define OLD_APP_PATH            "/switch/atmosphere-updater.nro"

int parseSearch(char *phare_string, char *filter, char* new_string);
int update_ams_hekate(char *url, char *output, int mode);
void update_app();

#endif