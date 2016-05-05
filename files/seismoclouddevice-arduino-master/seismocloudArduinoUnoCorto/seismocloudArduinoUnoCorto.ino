#include <Arduino.h>
#include <SoftwareSerial.h>
#include "common.h"
#include "ESP8266wifi.h"

#define sw_serial_rx_pin 4 //  Connect this pin to TX on the esp8266
#define sw_serial_tx_pin 6 //  Connect this pin to RX on the esp8266
#define esp8266_reset_pin 5 // Connect this pin to CH_PD on the esp8266, not reset. (let reset be unconnected)
#define SSID "OpenWrt"
#define PASSWORD "dorabino.7468!"

SoftwareSerial swSerial(sw_serial_rx_pin, sw_serial_tx_pin);
ESP8266wifi &wifi=ESP8266wifi::getWifi(swSerial, swSerial, esp8266_reset_pin);

unsigned long lastAliveMs = 0;
unsigned long lastProbeMs = 0;
uint32_t probeCount = 0;

void setup() {  
    Serial.begin(9600);
    bool wifi_started = wifi.begin();
    if (wifi_started) {
       wifi.connectToAP(SSID, PASSWORD);
  } else {
    // ESP8266 isn't working..
  }

    Serial.print(F("setup end\r\n"));

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
    Serial.println(wifi.getMAC());

    // Check Ethernet link
    Serial.print(F("My IP address: "));
    Serial.println(wifi.getIP());

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
    LED::green(true);
}

void loop() {
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
  probeCount++;
}

