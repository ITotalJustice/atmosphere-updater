#ifndef _UTIL_H_
#define _UTIL_H_

#define ROOT                    "/"
#define APP_PATH                "/switch/atmosphere-updater/"
#define AMS_OUTPUT              "/switch/atmosphere-updater/ams.zip"
#define HEKATE_OUTPUT           "/switch/atmosphere-updater/hekate.zip"
#define APP_OUTPUT              "/switch/atmosphere-updater/atmosphere-updater.nro"
#define OLD_APP_PATH            "/switch/atmosphere-updater.nro"

#define YES                     10
#define NO                      20
#define ON                      1
#define OFF                     0

void writeSysVersion();                                                 // writes the sys version.
void writeAmsVersion();                                                 // writes the ams version with hash.
char *getSysVersion();                                                  // returns sys version.
char *getAmsVersion();                                                  // returns ams version.

void copyFile(char *src, char *dest);                                   // basic copy file. Use malloc if you need dynamic mem
int parseSearch(char *phare_string, char *filter, char* new_string);    // hacky way to parse a file for a string
int update_ams_hekate(char *url, char *output, int mode);               // update either ams or hekate
void update_app();                                                      // update the app

#endif