#include <Arduino.h>
#include <SoftwareSerial.h>
#include "common.h"
#define SSID        "ITEAD"
#define PASSWORD    "12345678"
#define BAUD 9600
#define TX 2
#define RX 3

SoftwareSerial mySerial(RX, TX); /* RX:D3, TX:D2 */
ESP8266 wifi=MYWIFI::getWifi(mySerial,BAUD);


unsigned long lastAliveMs = 0;
unsigned long lastProbeMs = 0;
uint32_t probeCount = 0;


void setup() {  
    Serial.begin(9600);
    //Serial.print("setup begin\r\n");
 
    //Serial.print("FW Version:");
    //Serial.println(wifi.getVersion().c_str());
  
    
    if (wifi.setOprToStationSoftAP()) {
        Serial.print("to station + softap ok\r\n");
    } else {
        Serial.print("to station + softap err\r\n");
    }
 
    if (wifi.joinAP(SSID, PASSWORD)) {
        Serial.print("Join AP success\r\n");
        Serial.print("IP: ");
        Serial.println(wifi.getLocalIP().c_str());    
    } else {
        Serial.print("Join AP failure\r\n");
    }
    
    if (wifi.enableMUX()) {
        Serial.print("multiple ok\r\n");
    } else {
        Serial.print("multiple err\r\n");
    }
    
    Serial.print("setup end\r\n");

    LED::init(LED_GREEN, LED_YELLOW, LED_RED);
    LED::green(true);
    LED::red(true);
    LED::yellow(true);
  
    Serial.println(F("SeismoCloud-Arduino version "));
    Serial.println(getVersionAsString());

    checkEEPROM();

    // Check config, load MAC and lat/lon
    loadConfig();

    Serial.print(F("MAC Address: "));
    Serial.println(wifi.getLocalMAC());

    // Check Ethernet link
    Serial.print(F("My IP address: "));
    Serial.println(wifi.getLocalIP());

    Serial.println(F("Updating NTP Time"));
    do {
      updateNTP();
      setBootTime(getUNIXTime());
      if(getBootTime() == 0) {
        Serial.println(F("NTP update failed, retrying in 5s"));
        delay(5 * 1000);
      }
    } while(getBootTime() == 0);

    Serial.print(F("Local time is "));
    printUNIXTime();
    Serial.println();

    Serial.println(F("Init command interface"));
    commandInterfaceInit();
 
    Serial.println(F("Send first keep-alive to server..."));
    httpAliveRequest();
    lastAliveMs = millis();
/*
    if(getLatitude() == 0 && getLongitude() == 0) {
      LED::green(false);
      LED::red(false);
      LED::yellow(false);
      LED::setLedBlinking(LED_YELLOW);
      Serial.println(F("No position available, waiting for GPS from phone App"));
      do {
        commandInterfaceTick();
        LED::tick();
      } while(getLatitude() == 0 && getLongitude() == 0);
      LED::clearLedBlinking();
      LED::green(true);
      LED::red(true);
      LED::yellow(true);
    }
  
    Serial.print(F("GPS Latitude: "));
    Serial.print(getLatitudeAsString());
    Serial.print(F(" - Longitude: "));
    Serial.println(getLongitudeAsString());

    Serial.println(F("Init seismometer and calibrate"));
    seismometerInit();

    Serial.print(F("Boot completed at "));
    printUNIXTime();
    Serial.println();
    LED::startupBlink();
    LED::green(true);*/
}

void loop() {/*
  //Make sure the esp8266 is started..
  //if (!wifi.isStarted())
   // wifi.begin();
	
  // Update NTP (if necessary)
  updateNTP();

  commandInterfaceTick();
  LED::tick();

  // Calling alive every 14 minutes
  if((millis() - lastAliveMs) >= 840000) {
    Serial.print(F("Keepalive sent at "));
    printUNIXTime();
    Serial.println();
    
    httpAliveRequest();
    lastAliveMs = millis();
    
    Serial.print(F("Keepalive ACK at "));
    printUNIXTime();
    Serial.println();
  }

  // Detection
  seismometerTick();
  if(millis() - lastProbeMs >= 1000) {
    lastProbeMs = millis();
    setProbeSpeedStatistic(probeCount);
    probeCount = 0;
  }
  probeCount++;*/
}

