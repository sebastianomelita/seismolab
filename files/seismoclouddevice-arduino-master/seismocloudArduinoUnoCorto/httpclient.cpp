#include "httpclient.h"
//#include <utility/w5100.h>
//#include <utility/socket.h>
#include <avr/pgmspace.h>
//#define SYSLOG_PKT_SIZE 128

  void httpQuakeRequest() {
  char *server = "www.sapienzaapps.it";
  String postVars = String("deviceid=");
  char buf[5];
  postVars += String(ESP8266wifi::getWifi().getMAC());    
  postVars += "&tsstart=";
  postVars += getUNIXTime();
  postVars += "&lat=" + getLatitudeAsString() + "&lon=" + getLongitudeAsString();
  httpRequest(server, "80", "/seismocloud/terremoto.php", postVars, buf, "\r\n\r\n",true);
  Serial.print(F("\nQuake response: "));
  buf[1]=0;
  Serial.println(buf);
  Serial.println(F("\nEnd httpQuakeRequest")); 
}

void httpAliveRequest() {
  char *server = "www.sapienzaapps.it";
  Serial.println(F("\nBegin HttpAliveRequest"));
  char buf[100], sigma[10];

  String postVars = String("deviceid=");
  postVars += String(ESP8266wifi::getWifi().getMAC());
  postVars += "&model=uno";
  postVars += "&version=" + getVersionAsString();
  postVars += "&lat=" + getLatitudeAsString();
  postVars += "&lon=" + getLongitudeAsString();
  postVars += "&sensor=MPU6050";
  postVars += "&memfree="  + String(freeMemory()); 
  //postVars += "&avg=" + getDoubleAsString(getCurrentAVG());
  //postVars += "&stddev=" + getDoubleAsString(getCurrentSTDDEV());
  
  //postVars += "&jsonoutput=1"; //134 Bytes! attualmente il buffer MSG_BUFFER_MAX su ESP8266wifi.h � di 128 byte
  httpRequest(server, "80", "/seismocloud/alive.php", postVars, buf, "server:",true); 
  Serial.print(F("\nAliveResponse: "));
  Serial.println(buf);
  readParameter(buf,"sigma",sigma,10);
  if(atof(sigma)>0)
     setSigma(atof(sigma)); 
  else
     setSigma(11.33);
  Serial.println(F("\nEnd HttpAliveRequest"));
}

void logRequest(char* msg) {	
  char *server = "10.4.0.195";  // log server
  char msg2[115];	
  snprintf(msg2,115, "<134>[%lu] [I] [%s] %s", getUNIXTime(), ESP8266wifi::getWifi().getMAC(), msg);
  Serial.println(msg2);
  // Send a syslog request
  ESP8266wifi::getWifi().beginUDPPacket((const char*)server, "514"); // 514 is the syslog port
  ESP8266wifi::getWifi().write((const unsigned char*)msg2,strlen(msg2),128);
  ESP8266wifi::getWifi().endUDPPacket(false);
  ESP8266wifi::getWifi().disconnectFromServer();
  Serial.println(F("\nEnd logRequest")); 
}

void httpRequest(char* host, char* port, char* path, String &postVars, char * buf, char * offset, bool keepAlive){
  ESP8266wifi &client=ESP8266wifi::getWifi();
  // if there's a successful connection:
  int cresult = client.beginTCPConnection(host, port);
 
  if (cresult) {
    if(postVars == "") {
    	client.printD(F("\nGET "));
    } else {
        client.printD(F("\nPOST "));
    }
    client.printD(path);
    client.printlnD(F(" HTTP/1.1"));
    client.printD(F("Host: "));
    client.printlnD(host);
    client.printlnD(F("User-Agent: arduino-esp8266"));
    client.printD(F("Connection: "));
    if(keepAlive) {
    	client.printlnD(F("keep-alive"));
    } else {
        client.printlnD(F("closed"));
    }
    //client.println(F("User-Agent: arduino-ethernet"));
    if(postVars != "") {
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
    
    if(client.available(2000,offset)) {  //legge la risposta a partire dall stringa in offset 
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
  if(keepAlive) {
  	Serial.print(F("\nDisconnect HTTP From: "));
  	Serial.print(host);
  	Serial.print(F(":"));
  	Serial.print(port);
  	Serial.println(F(""));
  	client.disconnectFromServer();
  }
}


