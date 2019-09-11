#ifndef _UNZIP_H_
#define _UNZIP_H_

#define UP_AMS          0
#define UP_AMS_NCONFIG  1
#define UP_APP          2
#define REBOOT_PAYLOAD  3

int unzip(const char *output, int mode);

#endif