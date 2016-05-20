#include "httpclient.h"
//#include <utility/w5100.h>
//#include <utility/socket.h>
#include <avr/pgmspace.h>

void httpQuakeRequest() {
  String postVars = String("deviceid=");
  
  char * s, *d, *mac;
  for(mac=s=d=ESP8266wifi::getWifi().getMAC();*d=*s;d+=(*s++!=':')); //rimuove i :
  postVars += String(mac);
  
  postVars += "&tsstart=";
  postVars += getUNIXTime();
  postVars += "&lat=" + getLatitudeAsString() + "&lon=" + getLongitudeAsString();
  
  httpRequest(DEFAULTHOST, "80", "/seismocloud/terremoto.php", postVars);
}

void httpAliveRequest() {
  //String macaddress;
  Serial.println(F("\nBegin HttpAliveRequest"));
  
  char * s, *d, *mac;
  for(mac=s=d=ESP8266wifi::getWifi().getMAC();*d=*s;d+=(*s++!=':')); //rimuove i :
    // TODO: parametrized version and model
  String postVars = String("deviceid=");
  postVars += String(mac);
  postVars += "&model=uno";
  postVars += "&version=" + getVersionAsString();
  postVars += "&lat=" + getLatitudeAsString();
  postVars += "&lon=" + getLongitudeAsString();
  postVars += "&avg=" + String(getCurrentAVG());
  postVars += "&stddev=" + String(getCurrentSTDDEV());
  postVars += "&sensor=MPU6050";
  //postVars += "&memfree"=" + "10";
  httpRequest(DEFAULTHOST, "80", "/seismocloud/alive.php", postVars);
  Serial.println(F("\nEnd HttpAliveRequest"));
}

void httpRequest(char* host, char* port, char* path, String postVars) {
  char buf[50];
  ESP8266wifi &client=ESP8266wifi::getWifi();
  
  /*Serial.println(F("Read reply"));
  Serial.println(postVars);
  Serial.println(F("path"));
  Serial.println(path);*/
  
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
    client.println(F("User-Agent: arduino-wifi"));
    client.println(F("Connection: close"));
    //client.println(F("User-Agent: arduino-ethernet"));
    if(postVars != NULL) {
      client.println(F("Content-Type: application/x-www-form-urlencoded"));
      client.print(F("Content-Length: "));
	  String(postVars.length(), DEC).toCharArray(buf, 6);
      client.println(buf);
      client.println(F(""));
      client.print((char*)postVars.c_str());
    }else{
    	client.println(F("Content-Length: 0"));
		client.println(F(""));
	} 
    Serial.println(postVars);
    //unsigned long connms = millis();

    //while(!client.available() && millis() - connms < 10*1000);
   if(client.available(10*1000,NULL)) {
        client.readLine(buf,50);
		Serial.println(F("\nResponse"));
        Serial.println(buf);
    } else {
      Serial.println(F("Socket read error"));
    }
  } else {
    // if you couldn't make a connection:
    Serial.print(F("connection failed to: "));
    Serial.print(host);
    Serial.print(F(":"));
    Serial.print(port);
    Serial.print(F(" "));
    Serial.println(cresult);

    //ShowSockStatus();
  }
  //if(client.connected()) {
  //  client.stop();
  //}
  Serial.print(F("\nDisconnect HTTP From: "));
  Serial.print(host);
  Serial.print(F(":"));
  Serial.print(port);
  Serial.println(F(""));
  client.disconnectFromServer();
}

