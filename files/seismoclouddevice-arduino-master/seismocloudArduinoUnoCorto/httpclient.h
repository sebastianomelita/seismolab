#ifndef __httpclient_h
#define __httpclient_h

#include "common.h"
#define DEFAULTHOST "www.sapienzaapps.it"
#define DEFAULTHOST2 "ismarconicivitavecchia.tk"
//#define DEFAULTHOST "10.4.0.195"

void httpRequest(char* host, char* port, char* path, String &postVars , char * buf, char * offset, bool keepAlive=false);
void httpAliveRequest();
void httpQuakeRequest();
void ShowSockStatus();
#endif

