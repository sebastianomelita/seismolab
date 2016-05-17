#include "Commandinterface.h"

byte udpPacketBuffer[PACKET_SIZE];

void commandInterfaceInit() {
	Serial.println(F("beginUDPServer"));
    ESP8266wifi::getWifi().beginUDPServer("62001");
    //ESP8266wifi::getWifi().registerUDP( char* addr, char* port, char channel)
}

void commandInterfaceTick() {
  //Serial.println("commandInterfaceTick");	
  //int packetSize = ESP8266wifi::getWifi().parseUDPPacket(); //ci vorrebbe ma, se si legge tutto il buffer con una read, 
  //l'available � sufficiente per richiamare un nuovo paccchetto
  if(ESP8266wifi::getWifi().available(10, NULL, '0')) {
  	Serial.println(F("available"));
    // read the packet into packetBufffer
    size_t r = ESP8266wifi::getWifi().read((char*)udpPacketBuffer, PACKET_SIZE);
    Serial.print(F("Packet size: "));
    Serial.println(r);
    if(memcmp("INGV\0", udpPacketBuffer, 5) != 0) {
      return;
    }
    
    /*
    Serial.println((char)udpPacketBuffer[0],HEX);
    Serial.println((char)udpPacketBuffer[1],HEX);
    Serial.println((char)udpPacketBuffer[2],HEX);
    Serial.println((char)udpPacketBuffer[3],HEX);
    Serial.println((char)udpPacketBuffer[4],HEX);
    Serial.println((char)udpPacketBuffer[5],HEX);
    Serial.println((char)udpPacketBuffer[6],HEX);
    Serial.println((char)udpPacketBuffer[7],HEX);
    Serial.println((char*)udpPacketBuffer); */
    
    bool reboot = false;
    unsigned long unixTimeM = getUNIXTime();
    unsigned long uptime = getUNIXTime() - getBootTime();
    byte macaddress[6] = { 0 };
    char * s;
	char * d;
    for(s=d=ESP8266wifi::getWifi().getMAC();*d=*s;d+=(*s++!=':'));
    memcpy(macaddress, d, 6);
    uint32_t probeSpeed = getProbeSpeedStatistic();
    uint32_t freeramkb = freeMemory();
    float latency = 0;
    if(udpPacketBuffer[5] == PKTTYPE_GETINFO) {
      latency = tcpLatency();
    }

    float longitude = 0;
    float latitude = 0;
    /*
    Serial.println(F("Buffer[5]"));
    Serial.print((char)udpPacketBuffer[0]);
    Serial.print((char)udpPacketBuffer[1]);
    Serial.print((char)udpPacketBuffer[2]);
    Serial.print((char)udpPacketBuffer[2]);
    Serial.print((char)udpPacketBuffer[4]);
    Serial.print((char)udpPacketBuffer[5]);
    Serial.print((char)udpPacketBuffer[6]);
    Serial.println((char)udpPacketBuffer[7]);*/
    switch(udpPacketBuffer[5]) {
      case PKTTYPE_DISCOVERY:
      	
      	Serial.println(F("DISCOVERY"));
      	
        // Reply to discovery
        udpPacketBuffer[5] = PKTTYPE_DISCOVERY_REPLY;

        memcpy(udpPacketBuffer + 6, macaddress, 6);
        
        memcpy(udpPacketBuffer + 12, getVersionAsString().c_str(), 4);
        memcpy(udpPacketBuffer + 16, "uno", 3);
        break;
      case PKYTYPE_PING:
      	Serial.println(F("PING"));
        // Reply to ping
        udpPacketBuffer[5] = PKYTYPE_PONG;
        break;
      case PKTTYPE_SENDGPS:
      	Serial.println(F("SENDGPS"));
        // Get coords
        udpPacketBuffer[5] = PKTTYPE_OK;

        memcpy(&latitude, udpPacketBuffer + 12, 4);
        memcpy(&longitude, udpPacketBuffer + 16, 4);
        reverse4bytes((byte*)&latitude);
        reverse4bytes((byte*)&longitude);
        
        break;
      case PKTTYPE_REBOOT:
      	Serial.println(F("REBOOT"));
        // Reboot
        // Reply with OK
        udpPacketBuffer[5] = PKTTYPE_OK;
        reboot = true;
        break;
      case PKTTYPE_GETINFO:
      	Serial.println(F("GETINFO"));
        udpPacketBuffer[5] = PKTTYPE_GETINFO_REPLY;

        memcpy(udpPacketBuffer + 6, macaddress, 6);
        memcpy(udpPacketBuffer + 28, &uptime, 4);
        memcpy(udpPacketBuffer + 32, &unixTimeM, 4);
        memcpy(udpPacketBuffer + 36, VERSION, 4);
        memcpy(udpPacketBuffer + 40, &freeramkb, 4);
        memcpy(udpPacketBuffer + 44, &latency, 4);
        memcpy(udpPacketBuffer + 53, "uno", 3);
        memcpy(udpPacketBuffer + 57, "MPU6050", 7);
        memcpy(udpPacketBuffer + 65, &probeSpeed, 4);

        break;
#ifdef RESET_ENABLED
      case PKTTYPE_RESET:
        initEEPROM();
        reboot = true;
        break;
#endif
      default:
        // Unknown packet or invalid command
        Serial.println(F("INVALID COMMAND"));
        return;
    }

    if(longitude != 0 && latitude != 0) {
      setLongitude(longitude);
      setLatitude(latitude);
    }


    ESP8266wifi::getWifi().beginUDPPacket('0');
    ESP8266wifi::getWifi().write((unsigned char*) udpPacketBuffer,sizeof(udpPacketBuffer));
    //ESP8266wifi::getWifi().write((unsigned char*) "pippo",sizeof("pippo"));
    Serial.println(F("write"));
    ESP8266wifi::getWifi().endUDPPacket('0');

    /*if(reboot) {
      soft_restart();
    }*/
  }
}

