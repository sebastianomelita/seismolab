#ifndef __NTP_h
#define __NTP_h

#include "common.h"

unsigned long getUNIXTime();
unsigned long ntpUnixTime();
unsigned long updateNTP();
void printUNIXTime();
int beginPacket(uint8_t* host, uint32_t port);
int endPacket();
int write(uint8_t *buffer);
uint32_t parsePacket();
uint8_t read();
uint8_t read(uint8_t *buf);
#endif
