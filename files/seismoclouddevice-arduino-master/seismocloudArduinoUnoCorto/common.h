#ifndef __COMMON_H
#define __COMMON_H

#define LED_RED     3
#define LED_YELLOW  2
#define LED_GREEN   5
#define VERSION     "1.10"

//#define RESET_ENABLED
#include <EEPROM.h>
#include "LED.h"
#include "MemoryFree.h"
#include "MPU6050.h"
#include "SoftReset.h"
#include "utils.h"
#include "statistics.h"
#include "ESP8266WIFI.h"
#include "ntp.h"
#include "httpclient.h"
#include "Commandinterface.h"
#include "seismometer.h"

void initEEPROM();
void checkEEPROM();
float getLatitude();
String getLatitudeAsString();
String getLongitudeAsString();
float getLongitude();
unsigned long getBootTime();
void setBootTime(unsigned long);
void setLatitude(float l);
void setLongitude(float l);
void loadConfig();
void setProbeSpeedStatistic(uint32_t);
uint32_t getProbeSpeedStatistic();
String getVersionAsString();

#endif
