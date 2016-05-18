#include "httpclient.h"
//#include <utility/w5100.h>
//#include <utility/socket.h>
#include <avr/pgmspace.h>

void httpQuakeRequest() {
  String postVars = String("deviceid=");
  String macaddress;
  
  macaddress=ESP8266wifi::getWifi().getMAC();
  
  postVars += macaddress;
  
  postVars += "&tsstart=";
  postVars += getUNIXTime();
  postVars += "&lat=" + getLatitudeAsString() + "&lon=" + getLongitudeAsString();
  
  httpRequest(DEFAULTHOST, "80", "/seismocloud/terremoto.php", postVars);
}

void httpAliveRequest() {
  String postVars = String("deviceid=");
  String macaddress;
  
  macaddress=ESP8266wifi::getWifi().getMAC();
  postVars += macaddress;
  
  // TODO: parametrized version and model
  postVars += "&model=uno&version=" + getVersionAsString() + "&lat=" + getLatitudeAsString() + "&lon=" + getLongitudeAsString();
  httpRequest(DEFAULTHOST, "80", "/seismocloud/alive.php", postVars);
}

void httpRequest(char* host, char* port, char* path, String postVars) {
  char buf[6];
  ESP8266wifi &client=ESP8266wifi::getWifi();
  
  // if there's a successful connection:
  int cresult = client.beginTCPConnection(host, port);
 
  if (cresult) {
    if(postVars == NULL) {
      client.print(F("GET "));
    } else {
      client.print(F("POST "));
    }
    client.print(path);
    client.println(F(" HTTP/1.1"));
    
    client.print(F("Host: "));
    client.println(host);
    client.println(F("User-Agent: arduino-ethernet"));
    if(postVars != NULL) {
      client.println(F("Content-Type: application/x-www-form-urlencoded"));
      client.print(F("Content-Length: "));
	  String(postVars.length(), DEC).toCharArray(buf, 6);
      client.println(buf);
    }
    client.println(F("Connection: close"));
    //client.println("\n"); 
    
    if(postVars != NULL) {
      String(postVars.length(), DEC).toCharArray(buf, 6);
      client.println(buf);	
    }

    //unsigned long connms = millis();

    //while(!client.available() && millis() - connms < 10*1000);
   if(client.available(10*1000)) {
      // Read reply
      Serial.println(F("Read reply"));
      bool headerPass = false;
      while(client.available()) {
      	char buf[128+1];
        memset(buf, 0, 128+1);
        int r = client.readLine(buf, 128);
         Serial.println(F("Read reply2"));
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
    } else {
      Serial.println(F("Socket read error"));
    }
  } else {
    // if you couldn't make a connection:
    Serial.print(F("connection failed to: "));
    Serial.print(host);
    Serial.print(":");
    Serial.print(port);
    Serial.print(" ");
    Serial.println(cresult);

    //ShowSockStatus();
  }
  //if(client.connected()) {
  //  client.stop();
  //}
  Serial.println(F("Disconnect"));
    client.disconnectFromServer();
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
