// =====================================
// Author: Rocco Musolino - @roccomuso =
// =====================================

// Wifi Manager
#include <DNSServer.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
// I2C Accelerometer
#include <Wire.h> 

// NTP
#include <WiFiUdp.h>

unsigned int localPort = 2390;      // local port to listen for UDP packets
const char* ntpserver = "213.61.224.35"; // Default NTP server (updated from sapienzaapps.it responses)
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
WiFiUDP udp; // A UDP instance to let us send and receive packets over UDP

// WiFi Manager settings
const char* apName = "SeismoCloud-AP"; // AP wifi
char currentIp[24];

// Server HTTP Requests
const char* protocol = "http://";
const char* host = "www.sapienzaapps.it";
int httpPort = 80;
const char* KeepAliveUrl = "/seismocloud/alive.php";
const char* EarthQuakeUrl = "/seismocloud/terremoto.php";
unsigned long KeepAliveInterval = 900000; // calibration interval (every 15 minutes)
unsigned long previousKeepAliveMillis = 0; // Last time keepAlive message - do not edit.
unsigned long previousVibrationMillis = 0; // Last time vibration sent - do not edit
unsigned long vibrationInterval = 5; // in seconds, updated from server.
String lat = "41.892228";
String lon = "12.541938";
double VERSION = 1.0;
int jsonoutput = 1;

uint8_t MAC_array[6];
char MAC_char[18];

// MPU-6050 Accelerometer settings
const int MPU=0x68;  // I2C address of the MPU-6050 accelerometer
boolean ShakeFlag = false;
double Threshold = 0.0065;
double AccelerationFactor = 0.20 / 32768.0; // Assuming +/- 16G.
unsigned long interval = 30000; // calibration interval (every X milliseconds)

int16_t IX,IY,IZ,AcX,AcY,AcZ,Tmp;

double iX, iY, iZ, X, Y, Z;
double PeakForce = 0;

unsigned long previousMillis = 0; // last time calibration occurred
double CurrentForce = 0.0000;


void calibration(){
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,8,true);  // request a total of 8 registers
  
  IX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  IY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  IZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)

  iX = IX * AccelerationFactor;
  iY = IY * AccelerationFactor;
  iZ = IZ * AccelerationFactor;

}

void sendKeepAliveMex(){ // Send keepalive mex to the server
  
  if (WiFi.status() != WL_CONNECTED) return;


  // Concatenate URL
  // char fullUrl[7+strlen(host)+strlen(KeepAliveUrl)];
  // strcpy(fullUrl, protocol);
  // strcat(fullUrl, host);
  // strcat(fullUrl, KeepAliveUrl);

  Serial.println("Requesting KeepAliveUrl...");

  String mac_addr = String(MAC_char);
  Serial.println(mac_addr);
  mac_addr.replace(":", "%3A");

  String body = "deviceid="+mac_addr+
            "&model=esp8266&"+"lat="+lat+"&lon="+lon+
            "&version="+VERSION+"&jsonoutput="+jsonoutput;

HTTPClient http;
http.begin(host, httpPort, KeepAliveUrl);
http.addHeader("Content-Type", "application/x-www-form-urlencoded");
int httpCode = http.POST(body);
// httpCode will be negative on error: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/src/ESP8266HTTPClient.h#L36-L43
if(httpCode) {
    // HTTP header has been send and Server response header has been handled

  Serial.println("[HTTP] POST... done, code: "+String(httpCode));
  //Serial.println(ESP.getFreeHeap());
   // file found at server
   if(httpCode == 200) {
    // TODO: json processing can cause memory issue? no sembra sia anche soltanto giocare col payload.
       String payload = http.getString();
       char buf[payload.length()+1];
       payload.toCharArray(buf, payload.length()+1);
       // Read JSON reply from server, process it, and print to Serial. 
      Serial.println("Printing payload:");
      Serial.println(payload);
//http.writeToStream(&Serial);
         const int BUFFER_SIZE = JSON_OBJECT_SIZE(4); // ex. {"server":"www.sapienzaapps.it","ntpserver":"213.61.224.35","script":"","path":""}
         StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
        // // Deserialize the JSON string
         JsonObject& root = jsonBuffer.parseObject(buf); // get payload

         if (!root.success()){
           Serial.println("ArduinoJson parseObject() failed");
         }else{

            
             // Update server and ntpserver (taken from server keepalive response)
             const char* srv = root["server"];
             if (strcmp (srv, host) != 0) {
               host = strdup(srv); // TODO: server address should be saved and retrieved from EEPROM
             }
             const char* srvNtp = root["ntpserver"];
             if (strcmp (srvNtp, ntpserver) != 0){
               ntpserver = strdup(srvNtp);
             }

            Serial.println(srv);
            Serial.println(srvNtp);
            Serial.println(ntpserver);
         }

   }

} else {
  Serial.println("[HTTP] POST... failed, error: "+String(httpCode));
    //Serial.printf("[HTTP] POST... failed, error: %d\n", httpCode);
}

Serial.println("http END");
http.end();


}


void setup(){
  Wire.begin(4, 5); // sda, scl  // GPIO4 and GPIO5 - on Arduino: Wire.begin()
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(115200); // on Arduino: 9600

  WiFiManager wifi;
  wifi.setTimeout(120); // retry after 2 minutes to connect to a wifi.

  if(!wifi.autoConnect(apName)) { // timeut reached
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again
    ESP.reset();
    delay(5000);
  }

  // if you get here you have connected to the WiFi
  Serial.println("Wifi connected!");
  IPAddress myIp = WiFi.localIP(); // NB. not from WiFiManager lib but ESP8266WiFi.h
  sprintf(currentIp, "%d.%d.%d.%d", myIp[0], myIp[1], myIp[2], myIp[3]);
  // Getting MAC Address
  WiFi.macAddress(MAC_array);
    for (int i = 0; i < sizeof(MAC_array); ++i){
      sprintf(MAC_char,"%s%02x:",MAC_char,MAC_array[i]);
    }
  MAC_char[strlen(MAC_char)-1] = 0; // remove the trailing colon

  delay(2000); // wait 2 seconds before calibration
  calibration();
  Serial.println("First Calibration done.");

  // Initialize UDP for NTP
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());

  Serial.println("Host: " + String(host));
  Serial.println("Ntp Server:" + String(ntpserver));

  delay(1000);
  // Send first KeepAlive mex
  sendKeepAliveMex();

}


// function to send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet... NTP server: "+String(ntpserver));
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}


void vibrationDetected(){ // Send vibration detected alert to the server

  if (WiFi.status() != WL_CONNECTED) return;


  // Getting timestamp through NTP

  String _ntpserver = String(ntpserver);
  int commaIndex = _ntpserver.indexOf('.');
  int secondCommaIndex = _ntpserver.indexOf('.', commaIndex+1);
  int thirdCommaIndex = _ntpserver.indexOf('.', secondCommaIndex+1);
  String firstValue = _ntpserver.substring(0, commaIndex);
  String secondValue = _ntpserver.substring(commaIndex+1, secondCommaIndex);
  String thirdValue = _ntpserver.substring(secondCommaIndex+1, thirdCommaIndex); 
  String fourthValue = _ntpserver.substring(thirdCommaIndex+1); //To the end of the string

  IPAddress timeServerIP(firstValue.toInt(),secondValue.toInt(),thirdValue.toInt(),fourthValue.toInt());

  //Serial.println(timeServerIP);

  sendNTPpacket(timeServerIP); // send an NTP packet to a time server

  int cb = udp.parsePacket();
  int k = 0;
  while(!cb){
    Serial.println("No UDP NTP packet yet...");
    delay(20);
    k++;
    cb = udp.parsePacket();
    if (k > 150) break; // Timeout after 3 seconds.
  }

  String tsstart = "";
  if (k > 150){
    tsstart = "timeout";
    Serial.println("Timeout: No UDP NTP packet received.");
  }else{

    Serial.print("NTP packet received, length=");
    Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    //Serial.println(secsSince1900);
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.println("Unix Timestamp: "+String(epoch));
    // Convert to String
    tsstart = String(epoch);

  }

  // Concatenate URL
  // char fullUrl[7+strlen(host)+strlen(EarthQuakeUrl)];
  // strcpy(fullUrl, protocol);
  // strcat(fullUrl, host);
  // strcat(fullUrl, EarthQuakeUrl);

  Serial.println("Requesting EarthQuakeUrl...");

  // Sending Vibration detected alarm to the server
  String mac_addr = String(MAC_char);
  mac_addr.replace(":", "%3A");
  String body = "deviceid="+mac_addr+
            "&tsstart="+tsstart+"&lat="+lat+"&lon="+lon;

HTTPClient http;
http.begin(host, httpPort, EarthQuakeUrl);
http.addHeader("Content-Type", "application/x-www-form-urlencoded");
int httpCode = http.POST(body);
// httpCode will be negative on error: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/src/ESP8266HTTPClient.h#L36-L43
if(httpCode) {
    // HTTP header has been send and Server response header has been handled
    Serial.println(String(host)+" - "+String(httpPort)+" - [HTTP] POST... done, code: "+String(httpCode));

    // file found at server
    if(httpCode == 200) {
        String payload = http.getString();
        // Read reply from server, process it, and print to Serial. 
        Serial.println(payload);

        vibrationInterval = payload.toInt(); // time to wait before sending another vibration detected alert.

    }

} else {
    Serial.println("[HTTP] POST... failed, error: "+String(httpCode));
}


http.end();

}

void loop(){
  unsigned long currentMillis = millis();
  
// Initial Reading (Calibration)
  if (currentMillis - previousMillis > interval){
  
    previousMillis = currentMillis; // save last time calibration occurred
    calibration();
    Serial.println("Calibration Done!");
  
  }

// new Measurement
    
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
      Wire.endTransmission(false);
      Wire.requestFrom(MPU,8,true);  // request a total of 8 registers
      
      AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
      AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
      AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
      Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)

      //Serial.print("AcX = "); Serial.print(AcX);
      //Serial.print(" | AcY = "); Serial.print(AcY);
      //Serial.print(" | AcZ = "); Serial.println(AcZ);
  
      //delay(100);
      
      X = AcX * AccelerationFactor;
      Y = AcY * AccelerationFactor;
      Z = AcZ * AccelerationFactor;

      // Calculate force
      CurrentForce = sqrt(sq(X - iX) + sq(Y - iY) + sq(Z - iZ));
      // Serial.print(CurrentForce, 4); Serial.print(" - "); // FOR DEBUG
     

   if (CurrentForce >= Threshold){
      if (!ShakeFlag){
        ShakeFlag = true;
        Serial.println("Vibration Detected! ");
        // Sending HTTP Request...
          if (currentMillis - previousVibrationMillis > vibrationInterval * 1000){
              previousVibrationMillis = currentMillis; // save last time calibration occurred
              vibrationDetected();
            }

      }
   }else ShakeFlag = false;

  //Serial.print("AcX = "); Serial.print(AcX);
  //Serial.print(" | AcY = "); Serial.print(AcY);
  //Serial.print(" | AcZ = "); Serial.print(AcZ);
  //Serial.print(" | Tmp = "); Serial.println(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet

// KeepAlive message
  if (currentMillis - previousKeepAliveMillis > KeepAliveInterval){
  
    previousKeepAliveMillis = currentMillis; // save last time calibration occurred
    sendKeepAliveMex();
    Serial.println("KeepAlive mex sent!");
  
  }


  delay(100);
}
