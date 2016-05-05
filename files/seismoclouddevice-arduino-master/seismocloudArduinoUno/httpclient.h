#ifndef __httpclient_h
#define __httpclient_h

#include "common.h"
#define DEFAULTHOST "www.sapienzaapps.it"
//#define DEFAULTHOST "192.0.2.20"

void httpRequest(char* host, unsigned short port, char* path, String postVars);
void httpAliveRequest();
void httpQuakeRequest();
void print(String str);
void println(String str);
int readLine(uint8_t* buf);
void ShowSockStatus();
#endif
