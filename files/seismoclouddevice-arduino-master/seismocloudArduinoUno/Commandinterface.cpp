#include "Commandinterface.h"

ESP8266& wificmd= MYWIFI::getWifi();
byte udpPacketBuffer[PACKET_SIZE];

void commandInterfaceInit() {
    //wifi= MYWIFI::getWifi();
	if (wificmd.startServer(CMD_INTERFACE_PORT)) {
        	Serial.print("start tcp server ok\r\n");
    } else {
        	Serial.print("start tcp server err\r\n");
    }
}

void commandInterfaceTick() {
  uint8_t mux_id;

  uint32_t len = wificmd.recv(&mux_id, udpPacketBuffer, PACKET_SIZE,10);
  if (len > 0) {
		Serial.println("Message from config client:");
    	Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)udpPacketBuffer[i]);
        }
        Serial.print("]\r\n");
    	
    if(memcmp("INGV\0", udpPacketBuffer, 5) != 0) {
      return;
    }

    bool reboot = false;
    unsigned long unixTimeM = getUNIXTime();
    unsigned long uptime = getUNIXTime() - getBootTime();
    String str=wificmd.getLocalMAC();
    char macaddress[6] = { 0 };
    str.toCharArray(macaddress, 6);
    uint32_t probeSpeed = getProbeSpeedStatistic();
    uint32_t freeramkb = freeMemory();
    float latency = 0;
    if(udpPacketBuffer[5] == PKTTYPE_GETINFO) {
      latency = tcpLatency();
    }

    float longitude = 0;
    float latitude = 0;

    switch(udpPacketBuffer[5]) {
      case PKTTYPE_DISCOVERY:
        // Reply to discovery
        udpPacketBuffer[5] = PKTTYPE_DISCOVERY_REPLY;

        memcpy(udpPacketBuffer + 6, macaddress, 6);
        
        memcpy(udpPacketBuffer + 12, getVersionAsString().c_str(), 4);
        memcpy(udpPacketBuffer + 16, "uno", 3);
        break;
      case PKYTYPE_PING:
        // Reply to ping
        udpPacketBuffer[5] = PKYTYPE_PONG;
        break;
      case PKTTYPE_SENDGPS:
        // Get coords
        udpPacketBuffer[5] = PKTTYPE_OK;

        memcpy(&latitude, udpPacketBuffer + 12, 4);
        memcpy(&longitude, udpPacketBuffer + 16, 4);
        reverse4bytes((byte*)&latitude);
        reverse4bytes((byte*)&longitude);
        
        break;
      case PKTTYPE_REBOOT:
        // Reboot
        // Reply with OK
        udpPacketBuffer[5] = PKTTYPE_OK;
        reboot = true;
        break;
      case PKTTYPE_GETINFO:
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
        return;
    }

    if(longitude != 0 && latitude != 0) {
      setLongitude(longitude);
      setLatitude(latitude);
    }
    
    wificmd.send(mux_id,(const uint8_t*)udpPacketBuffer, PACKET_SIZE);
	
    if(reboot) {
      soft_restart();
    }
  }
}

