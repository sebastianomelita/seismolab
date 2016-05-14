#ifndef __NTP_h
#define __NTP_h

#include "common.h"

unsigned long getUNIXTime();
unsigned long ntpUnixTime();
unsigned long updateNTP();
void printUNIXTime();
bool NTPHttpRequest(char* host, char* port, char* path, char* UTP);
#endif
