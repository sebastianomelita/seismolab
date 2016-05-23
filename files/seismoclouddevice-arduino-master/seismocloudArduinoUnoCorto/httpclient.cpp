#include "httpclient.h"
//#include <utility/w5100.h>
//#include <utility/socket.h>
#include <avr/pgmspace.h>

void httpQuakeRequest() {
  String postVars = String("deviceid=");
  
  char buf[80];
  char * s, *d, *mac;
  for(mac=s=d=ESP8266wifi::getWifi().getMAC();*d=*s;d+=(*s++!=':')); //rimuove i :
  postVars += String(mac);
              
  postVars += "&tsstart=";
  postVars += getUNIXTime();
  postVars += "&lat=" + getLatitudeAsString() + "&lon=" + getLongitudeAsString();
  
  httpRequest(DEFAULTHOST, "80", "/seismocloud/terremoto.php", postVars, buf, NULL);
  Serial.print(F("\nQuake response: "));
  Serial.println(buf);
  Serial.println(F("\nEnd HttpAliveRequest")); 
}

void httpAliveRequest() {
  //String macaddress;
  Serial.println(F("\nBegin HttpAliveRequest"));
  
  char buf[80];
  char * s, *d, *mac;
  for(mac=s=d=ESP8266wifi::getWifi().getMAC();*d=*s;d+=(*s++!=':')); //rimuove i :
    // TODO: parametrized version and model
  String postVars = String("deviceid=");
  postVars += String(mac);
  //postVars += "00000000c1a0";
  postVars += "&model=uno";
  postVars += "&version=" + getVersionAsString();
  postVars += "&lat=" + getLatitudeAsString();
  postVars += "&lon=" + getLongitudeAsString();
  postVars += "&avg=" + String(getCurrentAVG());
  postVars += "&stddev=" + String(getCurrentSTDDEV());
  postVars += "&sensor=MPU6050";
  postVars += "&memfree="  + String(freeMemory()); //121 byte
  //postVars += "&jsonoutput=1"; //134 Bytes! attualmente il buffer MSG_BUFFER_MAX su ESP8266wifi.h è di 128 byte
  httpRequest(DEFAULTHOST, "80", "/seismocloud/alive.php", postVars, buf, "server:"); 
  Serial.print(F("\nResponse: "));
  Serial.println(buf);
  Serial.println(F("\nEnd HttpAliveRequest"));
}

void httpRequest(char* host, char* port, char* path, String postVars, char * buf, char * offset){
  ESP8266wifi &client=ESP8266wifi::getWifi();
  // if there's a successful connection:
  int cresult = client.beginTCPConnection(host, port);
 
  if (cresult) {
    if(postVars == NULL) {
    } else {
      client.printD(F("POST "));
    }
    client.printD(path);
    client.printlnD(F(" HTTP/1.1"));
    client.printD(F("Host: "));
    client.printlnD(host);
    client.printlnD(F("User-Agent: arduino-esp8266"));
    client.printlnD(F("Connection: close"));
    //client.println(F("User-Agent: arduino-ethernet"));
    if(postVars != NULL) {
      client.printlnD(F("Content-Type: application/x-www-form-urlencoded"));
      client.printD(F("Content-Length: "));
	  String(postVars.length(), DEC).toCharArray(buf, 80);
      client.printlnD(buf);
      client.printlnD(F(""));
      client.printD((char*)postVars.c_str());
    }else{
    	client.printlnD(F("Content-Length: 0"));
		client.printlnD(F(""));
	} 

   if(client.available(10*1000,offset)) {  //legge la risposta a partire dall stringa in offset 
        client.readLine(buf,80);
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
  }
  if(client.isConnectedToServer()) {
  	Serial.print(F("\nDisconnect HTTP From: "));
  	Serial.print(host);
  	Serial.print(F(":"));
  	Serial.print(port);
  	Serial.println(F(""));
  	client.disconnectFromServer();
  }
}


