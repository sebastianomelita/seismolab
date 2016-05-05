#include "httpclient.h"
//#include <utility/w5100.h>
//#include <utility/socket.h>

ESP8266& wifihttp= MYWIFI::getWifi();

void httpQuakeRequest() {
  String postVars = String("deviceid=");
  String macaddress;
  
  macaddress=wifihttp.getLocalMAC();
  
  postVars += macaddress;
  
  postVars += "&tsstart=";
  postVars += getUNIXTime();
  postVars += "&lat=" + getLatitudeAsString() + "&lon=" + getLongitudeAsString();
  
  httpRequest(DEFAULTHOST, 80, "/seismocloud/terremoto.php", postVars);
}

void httpAliveRequest() {
  String postVars = String("deviceid=");
  String macaddress;
  
  macaddress=wifihttp.getLocalMAC();
  postVars += macaddress;
  
  // TODO: parametrized version and model
  postVars += "&model=uno&version=" + getVersionAsString() + "&lat=" + getLatitudeAsString() + "&lon=" + getLongitudeAsString();
  httpRequest(DEFAULTHOST, 80, "/seismocloud/alive.php", postVars);
}

void httpRequest(char* host, unsigned short port, char* path, String postVars) {
  uint8_t mux_id;
  char numbuf[12];
  
  //wifi= MYWIFI::getWifi();
  // if there's a successful connection:
  wifihttp.createTCP(host, (uint32_t) port);
  
  if(postVars == NULL) {
      print(F("GET "));
    } else {
      print(F("POST "));
    }
    print(path);
    println(F(" HTTP/1.1"));

    print(F("Host: "));
    println(host);
    println(F("User-Agent: arduino-ethernet"));
    if(postVars != NULL) {
      println("Content-Type: application/x-www-form-urlencoded");
      print("Content-Length: ");
      itoa(postVars.length(),numbuf,10);
      println(numbuf);
    }
    println(F("Connection: close"));
    //println();

    if(postVars != NULL) {
      print(postVars);
    }
    
    uint8_t buffer[256] = {0};
    uint32_t len = wifihttp.recv(buffer, sizeof(buffer),10*1000);
    if (len > 0) {
      // Read reply
      bool headerPass = false;
      uint8_t buf[256+1];
      int r = readLine((uint8_t*)buf);
      while(r > 0) {
        memset(buf, 0, 256+1);
        int r = readLine((uint8_t*)buf);
        if(r < -1) break;
        if(headerPass) {
          // TODO: Read body
        } else if(r != 0) {
          // TODO: Read header
        } else {
          // Header separator
          headerPass = true;
        }
      }
    }else {
      Serial.println(F("Socket read error"));
    }
  /*} else {
    // if you couldn't make a connection:
    Serial.print(F("connection failed to: "));
    Serial.print(host);
    Serial.print(":");
    Serial.print(port);
    Serial.print(" ");
    Serial.println(cresult);*/

    //ShowSockStatus();
  
    wifihttp.releaseTCP();
}

/*void ShowSockStatus()
{
 Serial.println();
 for (int i = 0; i < MAX_SOCK_NUM; i++) {
   Serial.print(F("Socket#"));
   Serial.print(i);
   uint8_t s = W5100.readSnSR(i);
   Serial.print(F(":0x"));
   Serial.print(s,16);
   Serial.print(F(" "));
   Serial.print(W5100.readSnPORT(i));
   Serial.print(F(" D:"));
   uint8_t dip[4];
   W5100.readSnDIPR(i, dip);
   for (int j=0; j<4; j++) {
     Serial.print(dip[j],10);
     if (j<3) Serial.print(".");
   }
   Serial.print(F("("));
   Serial.print(W5100.readSnDPORT(i));
   Serial.println(F(")"));
 }
  Serial.println();
}*/

void print(String str){
	//wifi= MYWIFI::getWifi();
	char buf[20];
	str.toCharArray(buf, 20);
	wifihttp.send((const uint8_t*)buf, strlen(buf));
}

void println(String str){
	//wifi= MYWIFI::getWifi();
	str+="\n";
	char buf[20];
	str.toCharArray(buf, 20);
	wifihttp.send((const uint8_t*)buf, 20);
}

int readLine(uint8_t* buf) {
	//wifi= MYWIFI::getWifi();
    return wifihttp.recv(buf, 20, 10);
}

