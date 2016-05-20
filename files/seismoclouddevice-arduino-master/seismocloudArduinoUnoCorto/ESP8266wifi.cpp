//
  //  esp8266wifi.cpp
//
//
//  Created by Jonas Ekstrand on 2015-02-20.
//
//

#include "ESP8266wifi.h"

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

const char OK[] PROGMEM = "OK";
const char FAIL[] PROGMEM = "FAIL";
const char ERROR[] PROGMEM = "ERROR";
const char NO_CHANGE[] PROGMEM = "no change";

const char SEND_OK[] PROGMEM = "SEND OK";
const char LINK_IS_NOT[] PROGMEM = "link is not";
const char PROMPT[] PROGMEM = ">";
const char BUSY[] PROGMEM =  "busy";
const char LINKED[] PROGMEM = "Linked";
const char ALREADY[] PROGMEM = "ALREAY";//yes typo in firmware..
const char READY[] PROGMEM = "ready";
const char NO_IP[] PROGMEM = "0.0.0.0";

const char CIPSEND[] PROGMEM = "AT+CIPSEND=";
const char CIPSERVERSTART[] PROGMEM = "AT+CIPSERVER=1,";
const char CIPSERVERSTOP[] PROGMEM = "AT+CIPSERVER=0";
const char CIPSTART[] PROGMEM = "AT+CIPSTART=4,\""; 
const char CIPCLOSE[] PROGMEM = "AT+CIPCLOSE=4";
const char TCP[] PROGMEM = "TCP";
const char UDP[] PROGMEM = "UDP";

const char CWJAP[] PROGMEM = "AT+CWJAP=\"";

const char CWMODE_1[] PROGMEM = "AT+CWMODE=1";
const char CWMODE_3[] PROGMEM = "AT+CWMODE=3";
const char CWMODE_CHECK[] PROGMEM = "AT+CWMODE?";
const char CWMODE_OK[] PROGMEM = "+CWMODE:1";

const char CIFSR[] PROGMEM = "AT+CIFSR";
const char CIPMUX_1[] PROGMEM = "AT+CIPMUX=1";

const char ATE0[] PROGMEM = "ATE0";
const char ATE1[] PROGMEM = "ATE1";

const char CWSAP[] PROGMEM = "AT+CWSAP=\"";

const char IPD[] PROGMEM = "IPD,";
const char CONNECT[] PROGMEM = "CONNECT";
const char CLOSED[] PROGMEM = "CLOSED";

const char COMMA[] PROGMEM = ",";
const char COMMA_1[] PROGMEM = "\",";    
const char COMMA_2[] PROGMEM = "\",\""; 
const char THREE_COMMA[] PROGMEM = ",3";
const char DOUBLE_QUOTE[] PROGMEM = "\"";
const char EOL[] PROGMEM = "\n";

const char STAIP[] PROGMEM = "STAIP,\"";
const char STAMAC[] PROGMEM = "STAMAC,\"";
const char GMR[] PROGMEM = "AT+GMR";
const char dataModeOn[] PROGMEM = "Data mode Started";
const char dataModeOff[] PROGMEM = "Data mode Stopped";
//modifiche_____________________________________
const char CIPNTPSTART[] PROGMEM = "AT+CIPNTP=0";
const char CIPNTPSTOP[] PROGMEM = "AT+CIPNTP=1";
const char NTPTIME[] PROGMEM = "AT+CIPNTP?";
const char CIPMODEON[] PROGMEM = "AT+CIPMODE=1";
const char CIPMODEOFF[] PROGMEM = "AT+CIPMODE=0";
const char CIPMUX_0[] PROGMEM = "AT+CIPMUX=0";
const char CIPSEND_1[] PROGMEM = "AT+CIPSEND";
const char CIPSTART_1[] PROGMEM = "AT+CIPSTART=\"";
const char CIPSTART_2[] PROGMEM = "AT+CIPSTART=";
const char COMMA_3[] PROGMEM = ",\"";
const char TWO_COMMA[] PROGMEM = ",2";
const char NO_PORT[] PROGMEM = "0";
const char ENDL[] PROGMEM = "\n";
const char DATAEND[] PROGMEM = "+++";
//const char UDPNULL[] PROGMEM =  ",\"UDP\","\"0.0.0.0\",0,";
//--------------MODIFICHE ALLA LIBRERIA ORIGINALE-----------------------------
const char PEERIP[] PROGMEM = "AT+CIPSTATUS,\"";
//propriet� di classe
Stream* ESP8266wifi::_serialIn;
Stream* ESP8266wifi::_serialOut;
byte ESP8266wifi::_resetPin;
Stream* ESP8266wifi::_dbgSerial;
WifiMessage ESP8266wifi::msg;

//metodo di classe da invocare per tutte le chiamate successive alla prima
ESP8266wifi & ESP8266wifi::getWifi(){ 
		return getWifi(*_serialIn, *_serialOut,_resetPin, *_dbgSerial);
}
//-------------ORIGINALE----------------------------------------------

ESP8266wifi::ESP8266wifi(Stream &serialIn, Stream &serialOut, byte resetPin) {
    _serialIn = &serialIn;
    _serialOut = &serialOut;
    _resetPin = resetPin;
    
    pinMode(_resetPin, OUTPUT);
    digitalWrite(_resetPin, LOW);//Start with radio off
    
    flags.connectToServerUsingTCP = true;
    flags.endSendWithNewline = true;
    flags.started = false;
    flags.localServerConfigured = false;
    flags.localApConfigured = false;
    flags.apConfigured = false;
    flags.serverConfigured = false;
    flags.transparentMode= false;
    
    flags.debug = false;
    flags.echoOnOff = false;

    for (int i = 0; i < MAX_CONNECTIONS; i++) {
      _connections[i].channel = i + 0x30; // index to ASCII 
      _connections[i].connected = false;
    }
}

ESP8266wifi::ESP8266wifi(Stream &serialIn, Stream &serialOut, byte resetPin, Stream &dbgSerial) {
    _serialIn = &serialIn;
    _serialOut = &serialOut;
    _resetPin = resetPin;
    _dbgSerial = &dbgSerial;
    
    pinMode(_resetPin, OUTPUT);
    digitalWrite(_resetPin, LOW);//Start with radio off
    
    flags.connectToServerUsingTCP = true;
    flags.endSendWithNewline = false;
    flags.started = false;
    flags.localServerConfigured = false;
    flags.localApConfigured = false;
    flags.apConfigured = false;
    flags.serverConfigured = false;
    flags.transparentMode= false;
    
    _dbgSerial = &dbgSerial;
    flags.debug = true;
    flags.echoOnOff = true;    

    for (int i = 0; i < MAX_CONNECTIONS; i++) {
      _connections[i].channel = i + 0x30; // index to ASCII 
      _connections[i].connected = false;
    }
}


void ESP8266wifi::endSendWithNewline(bool endSendWithNewline){
    flags.endSendWithNewline = endSendWithNewline;
}

bool ESP8266wifi::begin() {
    msgOut[0] = '\0';
    msgIn[0] = '\0';
    flags.connectedToServer = false;
    flags.localServerConfigured = false;
    flags.localApConfigured = false;
    serverRetries = 0;
    pos=0;
    posw=0;
    
    //Do a HW reset
    bool statusOk = false;
    byte i;
    for(i =0; i<HW_RESET_RETRIES; i++){
        readCommand(10, NO_IP); //Cleanup
        digitalWrite(_resetPin, LOW);
        delay(500);
        digitalWrite(_resetPin, HIGH); // select the radio
        // Look for ready string from wifi module
        statusOk = readCommand(3000, READY) == 1;
        if(statusOk)
            break;
    }
    if (!statusOk)
        return false;
    
    //Turn local AP = off
    writeCommand(CWMODE_1, EOL);
    if (readCommand(1000, OK, NO_CHANGE) == 0)
        return false;
    
    // Set echo on/off
    if(flags.echoOnOff)//if echo = true
        writeCommand(ATE1, EOL);
    else
        writeCommand(ATE0, EOL);
    if (readCommand(1000, OK, NO_CHANGE) == 0)
        return false;
    
    // Set mux to enable multiple connections
    writeCommand(CIPMUX_1, EOL);
    flags.started = readCommand(3000, OK, NO_CHANGE) > 0;
    return flags.started;
}

bool ESP8266wifi::isStarted(){
    return flags.started;
}

bool ESP8266wifi::restart() {
    return begin()
        && (!flags.localApConfigured || startLocalAp())
        && (!flags.localServerConfigured || startLocalServer())
        && (!flags.apConfigured || connectToAP())
        && (!flags.serverConfigured || connectToServer());
}

bool ESP8266wifi::connectToAP(String& ssid, String& password) {
    return connectToAP(ssid.c_str(), password.c_str());
}

bool ESP8266wifi::connectToAP(const char* ssid, const char* password){//TODO make timeout config or parameter??
    strncpy(_ssid, ssid, sizeof _ssid);
    strncpy(_password, password, sizeof _password);
    flags.apConfigured = true;
    return connectToAP();
}

bool ESP8266wifi::connectToAP(){
    writeCommand(CWJAP);
    _serialOut -> print(_ssid);
    writeCommand(COMMA_2);
    _serialOut -> print(_password);
    writeCommand(DOUBLE_QUOTE, EOL);

    readCommand(15000, OK, FAIL);
    return isConnectedToAP();
}

bool ESP8266wifi::isConnectedToAP(){
    writeCommand(CIFSR, EOL);
    byte code = readCommand(350, NO_IP, ERROR);
    readCommand(10, OK); //cleanup
    return (code == 0);
}

char* ESP8266wifi::getIP(){
    msgIn[0] = '\0';
    writeCommand(CIFSR, EOL);
    byte code = readCommand(1000, STAIP, ERROR);
    if (code == 1) {
        // found staip
        readBuffer(&msgIn[0], sizeof(msgIn) - 1, '"');
        readCommand(10, OK, ERROR);
        return &msgIn[0];
    }
    readCommand(1000, OK, ERROR);
    return &msgIn[0];
}

char* ESP8266wifi::getMAC(){
    msgIn[0] = '\0';
    writeCommand(CIFSR, EOL);
    byte code = readCommand(1000, STAMAC, ERROR);
    if (code == 1) {
        // found stamac
        readBuffer(&msgIn[0], sizeof(msgIn) - 1, '"');
        readCommand(10, OK, ERROR);
        return &msgIn[0];
    }
    readCommand(1000, OK, ERROR);
    return &msgIn[0];
}

char* ESP8266wifi::getVersion(){
msgIn[0] = '\0';
    writeCommand(GMR, EOL);
    byte code = readCommand(1000,"", ERROR);
    if (code == 1) {
        // found stamac
        readBuffer(&msgIn[0], sizeof(msgIn) - 1, '"');
        readCommand(10, OK, ERROR);
        return &msgIn[0];
    }
    readCommand(1000, OK, ERROR);
    return &msgIn[0];
}

char* ESP8266wifi::getNTP(){
msgIn[0] = '\0';
    writeCommand(NTPTIME, EOL);
    byte code = readCommand(1000,"", ERROR);
    if (code == 1) {
        // found stamac
        readBuffer(&msgIn[0], sizeof(msgIn) - 1, '"');
        readCommand(10, OK, ERROR);
        return &msgIn[0];
    }
    readCommand(1000, OK, ERROR);
    return &msgIn[0];
}

bool ESP8266wifi::startNTPClient(){
    writeCommand(CIPNTPSTART, EOL);
    boolean stopped = (readCommand(2000, OK, NO_CHANGE) > 0);
    return stopped;
}

bool ESP8266wifi::stopNTPClient(){
    writeCommand(CIPNTPSTART, EOL);
    boolean stopped = (readCommand(2000, OK, NO_CHANGE) > 0);
    return stopped;
}

bool ESP8266wifi::startTransparentMode(){
	if(flags.localServerRunning){
		 writeCommand(CIPSERVERSTOP);
		 readCommand(5000, SEND_OK, BUSY);
	}
	writeCommand(CIPMUX_0, EOL);
	readCommand(5000, SEND_OK, BUSY);
	delay(1050);
	writeCommand(CIPMODEON, EOL);
	byte prompt = readCommand(5000, PROMPT, LINK_IS_NOT);
    if(1){
     	flags.transparentMode= true;
     	Serial.println(dataModeOn);
     	 return 1;
	 }	
    return 0;
}

bool ESP8266wifi::stopTransparentMode(){
	if(flags.transparentMode){
        	_serialOut -> println();
        	readCommand(2000, SEND_OK, BUSY);
        	delay(1050);
            _serialOut -> println(DATAEND);
            delay(1050);
	}
	if(flags.localServerRunning){
		writeCommand(CIPSERVERSTART, EOL);
	    readCommand(5000, SEND_OK, BUSY);	
	}
    writeCommand(CIPMODEOFF, EOL);
    readCommand(5000, SEND_OK, BUSY);
    writeCommand(CIPMUX_1, EOL);
    readCommand(5000, OK, NO_CHANGE);
    if(1){
    	flags.transparentMode= false;
    	Serial.println(dataModeOff);
    	 return 1;
	}	
    return 0;
}

void ESP8266wifi::setTransportToUDP(){
    flags.connectToServerUsingTCP = false;
}

void ESP8266wifi::setTransportToTCP(){
    flags.connectToServerUsingTCP = true;
}

bool ESP8266wifi::connectToServer(String& ip, String& port, bool peerChange) {
    return connectToServer(ip.c_str(), port.c_str(), peerChange);
}

bool ESP8266wifi::connectToServer(const char* ip, const char* port, bool peerChange){//TODO make timeout config or parameter??
    strncpy(_ip, ip, sizeof _ip);
    strncpy(_port, port, sizeof _port);
    flags.serverConfigured = true;
    return connectToServer(peerChange);
}


bool ESP8266wifi::connectToServer(bool peerChange) {
    if(peerChange)
	    writeCommand(CIPSTART);
	else
	    writeCommand(CIPSTART_1);
	    
    if (flags.connectToServerUsingTCP)
        writeCommand(TCP);
    else
        writeCommand(UDP);
        
     writeCommand(COMMA_2);
    _serialOut -> print(_ip);
    writeCommand(COMMA_1);
    //_serialOut -> println(_port);
	_serialOut -> println(_port);   
    flags.connectedToServer = (readCommand(10000, OK, ALREADY) > 0);
    
    if(flags.connectedToServer)
        serverRetries = 0;
    return flags.connectedToServer;
}


void ESP8266wifi::disconnectFromServer(){
    flags.connectedToServer = false;
    flags.serverConfigured = false;//disable reconnect
    writeCommand(CIPCLOSE, EOL);
    readCommand(5000, OK, ERROR);
}


bool ESP8266wifi::isConnectedToServer(){
    if(flags.connectedToServer)
        serverRetries = 0;
    return flags.connectedToServer;
}

bool ESP8266wifi::startLocalAPAndServer(const char* ssid, const char* password, const char* channel, const char* port){
    strncpy(_localAPSSID, ssid, sizeof _localAPSSID);
    strncpy(_localAPPassword, password, sizeof _localAPPassword);
    strncpy(_localAPChannel, channel, sizeof _localAPChannel);
    strncpy(_localServerPort, port, sizeof _localServerPort);
    
    flags.localApConfigured = true;
    flags.localServerConfigured = true;
    return startLocalAp() && startLocalServer();
}

bool ESP8266wifi::startLocalAP(const char* ssid, const char* password, const char* channel){
    strncpy(_localAPSSID, ssid, sizeof _localAPSSID);
    strncpy(_localAPPassword, password, sizeof _localAPPassword);
    strncpy(_localAPChannel, channel, sizeof _localAPChannel);

    flags.localApConfigured = true;
    return startLocalAp();
}

bool ESP8266wifi::startLocalServer(const char* port) {
    strncpy(_localServerPort, port, sizeof _localServerPort);
    flags.localServerConfigured = true;
    return startLocalServer();
}

bool ESP8266wifi::startLocalServer(){
    // Start local server
    writeCommand(CIPSERVERSTART);
    _serialOut -> println(_localServerPort);
    
    flags.localServerRunning = (readCommand(2000, OK, NO_CHANGE) > 0);
    return flags.localServerRunning;
}

bool ESP8266wifi::startLocalAp(){
    // Start local ap mode (eg both local ap and ap)
    writeCommand(CWMODE_3, EOL);
    if (!readCommand(2000, OK, NO_CHANGE))
        return false;
    
    // Configure the soft ap
    writeCommand(CWSAP);
    _serialOut -> print(_localAPSSID);
    writeCommand(COMMA_2);
    _serialOut -> print(_localAPPassword);
    writeCommand(COMMA_1);
    _serialOut -> print(_localAPChannel);
    writeCommand(THREE_COMMA, EOL);
    
    flags.localApRunning = (readCommand(5000, OK, ERROR) == 1);
    return flags.localApRunning;
}

bool ESP8266wifi::stopLocalServer(){
    writeCommand(CIPSERVERSTOP, EOL);
    boolean stopped = (readCommand(2000, OK, NO_CHANGE) > 0);
    flags.localServerRunning = !stopped;
    flags.localServerConfigured = false; //to prevent autostart
    return stopped;
}

bool ESP8266wifi::stopLocalAP(){
    writeCommand(CWMODE_1, EOL);

    boolean stopped = (readCommand(2000, OK, NO_CHANGE) > 0);
    flags.localApRunning = !stopped;
    flags.localApConfigured = false; //to prevent autostart
    return stopped;
}

bool ESP8266wifi::stopLocalAPAndServer(){
    return stopLocalAP() && stopLocalServer();
}

bool ESP8266wifi::isLocalAPAndServerRunning(){
    return flags.localApRunning & flags.localServerRunning;
}

// Performs a connect retry (or hardware reset) if not connected
bool ESP8266wifi::watchdog() {
    if (serverRetries >= SERVER_CONNECT_RETRIES_BEFORE_HW_RESET) {
        // give up, do a hardware reset
        return restart();
    }
    if (flags.serverConfigured && !flags.connectedToServer) {
        serverRetries++;
        if (flags.apConfigured && !isConnectedToAP()) {
            if (!connectToAP()) {
                // wait a bit longer, then check again
                delay(2000);
                if (!isConnectedToAP()) {
                    return restart();
                }
            }
        }
        return connectToServer();
    }
    return true;
}

/*
 * Send string (if channel is connected of course)
 */
bool ESP8266wifi::send(char channel, String& message, bool sendNow) {
    return send(channel, message.c_str(), sendNow);
}

bool ESP8266wifi::send(char channel, const char * message, bool sendNow){
    //watchdog();
    byte avail = sizeof(msgOut) - strlen(msgOut) - 1;
    strncat(msgOut, message, avail);
    if (!sendNow)
        return true;
    byte length = strlen(msgOut);
    
    if(flags.endSendWithNewline)
        length += 2;
    //Serial.print(F("message: "));
    //Serial.println(message);
    //Serial.print(F("msgOut: "));
    //Serial.println(msgOut);
    //Serial.print(F("length: "));
    //Serial.println(length);
    
    writeCommand(CIPSEND);
    _serialOut -> print(channel);
    writeCommand(COMMA);
    _serialOut -> println(length);
    byte prompt = readCommand(1000, PROMPT, LINK_IS_NOT);
    if (prompt != 2) {
        if(flags.endSendWithNewline)
            _serialOut -> println(msgOut);
        else
            _serialOut -> print(msgOut);
        byte sendStatus = readCommand(5000, SEND_OK, BUSY);
        Serial.println(msgOut);
        if (sendStatus == 1) {
            msgOut[0] = '\0';
            if(channel == SERVER)
                flags.connectedToServer = true;
            return true;
        }
    }
    //else
    if(channel == SERVER)
        flags.connectedToServer = false;
    else 
        _connections[channel-0x30].connected = false;
    msgOut[0] = '\0';
    return false;
}

// Checks to see if there is a client connection
bool ESP8266wifi::isConnection(void) {
    WifiConnection *connections;

    // return the first channel, assume single connection use
    return checkConnections(&connections);
}

// Updates private connections struct and make passed pointer point to data
bool ESP8266wifi::checkConnections(WifiConnection **pConnections) {
    watchdog();
    // setup buffers on stack & copy data from PROGMEM pointers
    char buf1[16] = {'\0'};
    char buf2[16] = {'\0'};
    char buf3[16] = {'\0'};
    strcpy_P(buf1, CONNECT);
    strcpy_P(buf2, READY);
    strcpy_P(buf3, CLOSED);
    byte len1 = strlen(buf1);
    byte len2 = strlen(buf2);
    byte len3 = strlen(buf3);
    byte pos = 0;
    byte pos1 = 0;
    byte pos2 = 0;
    byte pos3 = 0;
    byte ret = 0;
    char ch = '-';

    // unload buffer and check match
    while (_serialIn->available()) {
        char c = readChar();
        // skip white space
        if (c != ' ') {
          // get out of here if theres a message
          if (c == '+')
            break;
          // first char is channel
          if (pos == 0)
            ch = c;
          pos++;
          pos1 = (c == buf1[pos1]) ? pos1 + 1 : 0;
          pos2 = (c == buf2[pos2]) ? pos2 + 1 : 0;
          pos3 = (c == buf3[pos3]) ? pos3 + 1 : 0;
          if (len1 > 0 && pos1 == len1) {
              ret = 1;
              break;
          }
          if (len2 > 0 && pos2 == len2) {
              ret = 2;
              break;
          }
          if (len3 > 0 && pos3 == len3) {
              ret = 3;
              break;
          }
        }
    }

    if (ret == 2)
      //restart();

    // new connection
    if (ret == 1) {
      _connections[ch-0x30].connected = true;
      *pConnections = _connections;
      if (ch == SERVER)
          flags.connectedToServer = true;
      return 1;
    } 

    // channel disconnected
    if (ret == 3) {
      _connections[ch-0x30].connected = false;
      *pConnections = _connections;
      if (ch == SERVER)
          flags.connectedToServer = false;
      return 0;
    }

    // nothing has changed return single connection status
    *pConnections = _connections;
    return _connections[0].connected;
}

WifiMessage ESP8266wifi::listenForIncomingMessage(int timeout, char *from){
    //watchdog();
    char buf[16] = {'\0'};
    msgIn[0] = '\0';
    pos=0;
    offset=0;
    //static WifiMessage msg;
    
    msg.hasData = false;
    msg.channel = '-';
    msg.message = msgIn;
    msg.length = 0;

    //TODO listen for unlink etc...
    byte msgOrRestart = readCommand(timeout, IPD, READY);
      //Detected a esp8266 restart
    if (msgOrRestart == 2){
        //restart();
        return msg;
    }
    //Message received..
    else if (msgOrRestart == 1) {
        char channel = readChar();
        if (channel == SERVER)
            flags.connectedToServer = true;
        readChar(); // removing comma
        readBuffer(&buf[0], sizeof(buf) - 1, ':'); // read char count
        readChar(); // removing ':' delim
		uint16_t length = atoi(buf);
		//Serial.print("From channel: "); 
		//Serial.println(channel); 
		//Serial.print("length: "); 
		//Serial.println(length); 
        if(from!=NULL)
            offset = getFrom(from);
		//Serial.print("offset: "); 
		//Serial.println(offset); 
        //if(readBuffer(&msgIn[0], min(length-offset, MSG_BUFFER_MAX - 1),'\0')<length-offset)
        	//rxEmpty(); //se non entra tutto il messaggio leggi la rimanenza fino alla fine senza memorizzarla     
        readBuffer2(&msgIn[0], min(length-offset, MSG_BUFFER_MAX - 1));
		msg.hasData = true;
        msg.channel = channel;
        msg.message = msgIn;
        msg.length=min(length-offset, MSG_BUFFER_MAX - 1);
        //Serial.print("hasData: ");
	    //Serial.println(msg.hasData);
        readCommand(10, OK); // cleanup after rx
    }
    //Serial.println("No response");
    return msg;
}

WifiMessage ESP8266wifi::getIncomingMessage(char *from) {
    //watchdog();
    char buf[16] = {'\0'};
    msgIn[0] = '\0';
    pos=0;
    offset=0;
    //static WifiMessage msg;  variante all'originale
    
    msg.hasData = false;
    msg.channel = '-';
    msg.message = msgIn;
    msg.length = 0;
    
    // See if a message has come in (block 1s otherwise misses?)
    byte msgOrRestart = readCommand(10, IPD, READY);
     //Detected a esp8266 restart
    if (msgOrRestart == 2){
        //restart();
        return msg;
    }
    //Message received..
    else if (msgOrRestart == 1) {
        char channel = readChar();
        if (channel == SERVER)
            flags.connectedToServer = true;
        readChar(); // removing comma
        readBuffer(&buf[0], sizeof(buf) - 1, ':'); // read char count
        readChar(); // removing ':' delim
        uint16_t length = atoi(buf);
        //Serial.print("length2: "); 
		//Serial.println(length);
        if(from!=NULL)
            offset = getFrom(from);  
        //Serial.println(offset);
        //if(readBuffer(&msgIn[0], min(length-offset, MSG_BUFFER_MAX - 1),'\0')<length-offset)
        	//rxEmpty(); //se non entra tutto il messaggio leggi la la rimanenza fino alla fine senza memorizzarla     
        readBuffer2(&msgIn[0], min(length-offset, MSG_BUFFER_MAX - 1));
		msg.hasData = true;
        msg.channel = channel;
        msg.message = msgIn;
        msg.length=min(length-offset, MSG_BUFFER_MAX - 1);
        readCommand(10, OK); // cleanup after rx
    }
    return msg;
}

// Writes commands (from PROGMEM) to serial output
void ESP8266wifi::writeCommand(const char* text1 = NULL, const char* text2) {
    char buf[16] = {'\0'};
    strcpy_P(buf, (char *) text1);
    _serialOut->print(buf);
    if (text2 == EOL) {
        _serialOut->println();
    } else if (text2 != NULL) {
        strcpy_P(buf, (char *) text2);
        _serialOut->print(buf);
    }
    
}

// Reads from serial input until a expected string is found (or until timeout)
// NOTE: strings are stored in PROGMEM (auto-copied by this method)
byte ESP8266wifi::readCommand(int timeout, const char* text1, const char* text2) {
    // setup buffers on stack & copy data from PROGMEM pointers
    char buf1[16] = {'\0'};
    char buf2[16] = {'\0'};
    if (text1 != NULL)
        strcpy_P(buf1, (char *) text1);
    if (text2 != NULL)
        strcpy_P(buf2, (char *) text2);
    byte len1 = strlen(buf1);
    byte len2 = strlen(buf2);
    byte pos1 = 0;
    byte pos2 = 0;

    // read chars until first match or timeout
    unsigned long stop = millis() + timeout;
    do {
        while (_serialIn->available()) {
            char c = readChar();

            if(pos1<len1)
                if(c == buf1[pos1])
                    pos1++;
                else if(c == buf1[0])
                    pos1=1;
                else 
                    pos1=0;
          
			if(pos2<len2)
                if(c == buf2[pos2])
                    pos2++;
                else if(c == buf2[0])
                    pos2=1;
                else 
                    pos2=0;
                    
            if (len1 > 0 && pos1 == len1)
            	return 1;
            else if (len2 > 0 && pos2 == len2)
        		return 2;
        }
        delay(10);
    } while (millis() < stop);
    //Serial.println(pos1);
    //Serial.println(pos2);
    return 0;
}

// Unload buffer without delay
/*byte ESP8266wifi::readCommand(const char* text1, const char* text2) {
    // setup buffers on stack & copy data from PROGMEM pointers
    char buf1[16] = {'\0'};
    char buf2[16] = {'\0'};
    if (text1 != NULL)
        strcpy_P(buf1, (char *) text1);
    if (text2 != NULL)
        strcpy_P(buf2, (char *) text2);
    byte len1 = strlen(buf1);
    byte len2 = strlen(buf2);
    byte pos1 = 0;
    byte pos2 = 0;

    // read chars until first match or timeout
    while (_serialIn->available()) {
        char c = readChar();
        pos1 = (c == buf1[pos1]) ? pos1 + 1 : 0;
        pos2 = (c == buf2[pos2]) ? pos2 + 1 : 0;
        if (len1 > 0 && pos1 == len1)
            return 1;
        if (len2 > 0 && pos2 == len2)
            return 2;
    }
    return 0;
}*/

// Reads count chars to a buffer, or until delim char is found then add a null char
uint16_t ESP8266wifi::readBuffer(char* buf, uint16_t count, char delim) {
    uint16_t pos1 = 0;
  
    //unsigned long start;     
	//Serial.print("\ndaqua leggo: "); 
	//Serial.println(count);   
	//start = millis();
	
	//count conta a meno del \0
	//while (millis() - start < 3000 && pos < count) {
    while (_serialIn->available() && pos1 < count) {
        if (_serialIn->peek() == delim)
            break;
        buf[pos1++] = readChar();
    }
    buf[pos1] = '\0';
    //	}
    //Serial.print("buf: ");
	//Serial.print(buf);
	return pos1; //conta anche il \0
}

uint16_t ESP8266wifi::readBuffer2(char* buf, uint16_t count) {
    uint16_t pos1 = 0;
  
    while (_serialIn->available() && pos1 < count) {
        buf[pos1++] = readChar();
    }
	return pos1; //conta anche il \0
}

// Reads a single char from serial input (with debug printout if configured)
char ESP8266wifi::readChar() {
    char c = _serialIn->read();
    if (flags.debug)
        _dbgSerial->print(c);
    else
        delayMicroseconds(50); // don't know why
    return c;
}

void ESP8266wifi::rxEmpty(){
	while(_serialIn->available() > 0) {
        _serialIn->read();
    }
}

uint16_t ESP8266wifi::getFrom(char *buf){
byte pos1;
uint16_t p;
           
    byte len = strlen(buf);

    for(p=0, pos1=0; _serialIn->available() && p < 2048; p++) {
        char c = readChar();
        
		if(pos<len)
        	if(c == buf[pos1])
            	pos1++;
        	else if(c == buf[0])
            	pos1=1;
        	else 
            	pos1=0;      
    	if (len > 0 && pos1 == len)
            return p;
    }
    return 0;
}

//AGGIUNTE ALLA LIBRERIA ORIGINALE (METODI ANLOGHI ALLA LIBRERIA EthernetClient() di Arduino) --------------

bool ESP8266wifi::beginUDPPacket(const char* host,const char* port, bool transparent){ 
    bool app;
    setTransportToUDP(); 
    endSendWithNewline(false);
    posw=0;
    pos=0;
    msg.first=true;
    if(!isConnectedToServer()){
        if(transparent){
        	startTransparentMode();
        	//writeCommand(CIPMODEON, EOL);
	        //readCommand(2000, OK, NO_CHANGE);
            app = connectToServer(host,port,false);
        }else{
         	app = connectToServer(host,port,true);
		}
        return app;  
    }else 
        if(transparent)
            startTransparentMode();
            //writeCommand(CIPMODEON, EOL);
	        //readCommand(2000, OK, NO_CHANGE);
        return 0;
}

bool ESP8266wifi::beginTCPConnection(const char* host,const char* port){ 
    setTransportToTCP(); 
    return connectToServer(host,port);
    pos=-1;
}

bool ESP8266wifi::beginUDPPacket(char channel){ 
    setTransportToUDP(); 
    endSendWithNewline(false);
    posw=0;
    pos=0;
    return true;
}

bool ESP8266wifi::endUDPPacket(char channel){
	byte sendStatus;
	byte prompt;
	//Serial.println(F("length"));
    //Serial.println(posw);
    if(flags.endSendWithNewline)
        posw += 2;
    
    if(!flags.transparentMode){
    	//invia il canale e la lunghezza
    	writeCommand(CIPSEND);
    	_serialOut -> print(channel);
        writeCommand(COMMA);
        _serialOut -> println(posw);
        prompt = readCommand(1000, PROMPT, LINK_IS_NOT);
	}else if(msg.first){
	    writeCommand(CIPSEND_1); //vale per il primo pacchetto e non invia canale e lunghezza
	    msg.first=false;
	    //prompt = readCommand(1000, PROMPT, LINK_IS_NOT);
	    prompt = 1;
    }

	if (prompt != 2) {
        if(flags.endSendWithNewline)
            _serialOut -> println(msgOut);
        else
		   	_serialOut -> write(msgOut,posw);
	   
        if(!flags.transparentMode)
		    byte sendStatus = readCommand(5000, SEND_OK, BUSY);
		else
		    byte sendStatus = readCommand(30, SEND_OK, BUSY);    
        
		//if(sendStatus == 1 || flags.transparentMode) {
        //Serial.print("posw: ");
		//Serial.println(posw);
		//Serial.print("msgOut: ");
		//Serial.println(msgOut);
		msgOut[0] = '\0';
        if(channel == SERVER)
                flags.connectedToServer = true;
        //return true;
        //}
    }
    posw=0; //azzero il segnaposto (indica la prima posizione da accedere)
    //setTransportToTCP();
    return true;
}

int ESP8266wifi::write(const unsigned char* buf, size_t size){
	//for(int i=0;i<size;i++)
	//    msgOut[posw+i]=0;
    memcpy((char *)msgOut+posw, (const char *)buf, size);
	posw+=size;	
    return size;
}

unsigned char ESP8266wifi::write(const unsigned char buf){
	msgOut[posw++];	
    return buf;
}

int ESP8266wifi::parseUDPPacket(int timeout, char *from){
	//getIncomingMessage(); //messaggio memorizzato in msgIn[]
	listenForIncomingMessage(timeout);
	pos=0;
	//if(msg.hasData && msg.channel==channel || msg.hasData && channel == ANYCLIENT){
	if(msg.hasData){
        return msg.length;
    }
    return 0;
}

char ESP8266wifi::read(){
if(pos<msg.length && msg.hasData)
   return msgIn[pos++];
else
   return msgIn[pos-1];
}

char ESP8266wifi::readTCP(char *nextFrom){	
	    /*Serial.print("pos: ");
        Serial.print(pos);
        Serial.print("length: ");
        Serial.print(msg.length);*/
        char ch=msg.channel;
        if(pos<msg.length && msg.hasData){
			return msgIn[pos++];
		}else if(getIncomingMessage(nextFrom).hasData && msg.channel == ch){
				pos=0;
		    	return msgIn[pos++];
		}else
		    return 0;	
}

size_t ESP8266wifi::read(char* buf, size_t size){
if(pos+size<msg.length && msg.hasData){
   memcpy((char *)buf, (const char *)(msgIn+pos), size);
   pos+=size;
   return size;	
}
else
{
   memcpy(buf, msgIn+pos, MSG_BUFFER_MAX-pos-1);
   pos=MSG_BUFFER_MAX-1;
   return MSG_BUFFER_MAX-pos-1;	
}
}

void ESP8266wifi::print(char* str, char channel){
	send(channel,str,true);//da cambiare
}

void ESP8266wifi::println(char* str, char channel){
	endSendWithNewline(true);
	send(channel,str,true);
	endSendWithNewline(false);
}

void ESP8266wifi::println(char channel){
	send(channel,ENDL,true);
}

void ESP8266wifi::print(const __FlashStringHelper *s, char channel){
	PGM_P p = reinterpret_cast<PGM_P>(s);
	posw=0;
	
	while (1)
	{
		unsigned char c = pgm_read_byte(p++);
		//Serial.print((char)c);
		if(c == 0)
		{
			msgOut[posw] = c;
			if(posw > 0){
				Serial.println(msgOut);
				endUDPPacket(channel);
				//posw=0;
			}	
			return;
		}
		msgOut[posw++] = c;
		if(posw >= MSG_BUFFER_MAX) 
		{ 
			endUDPPacket(channel);
		}
	}	
}
	
void ESP8266wifi::println(const __FlashStringHelper *s, char channel){
	endSendWithNewline(true);
	print(s, channel);
	endSendWithNewline(false);
	//Serial.println(msgOut);
}

int ESP8266wifi::readLine(char* buf, size_t bufmax) {
    int i;
    //busmax must be <= MSG_BUFFER_MAX
    for(i=0; msg.hasData && i < bufmax ; i++) {
        buf[i] = readTCP();
        //Serial.print(buf[i]);
        //Serial.print(',');
          
		if(buf[i] == 10 || buf[i] == 0){ //(LF e NULL char) ritorna
		    buf[i] = 0;
            return i;
    	}
        else if(buf[i] == 13) 
            i--; //(CR) non lo copia 
    }
    
    if(i == bufmax){
    	buf[i-1]=0;
    	//Serial.print("bufmax");
        return bufmax;	
	}
	return 0;
}

char ESP8266wifi::getCurrLinkId(){
	return msg.channel;
}

int ESP8266wifi::available(int timeoutMillis, char *from, char channel){
	if(msg.hasData && msg.channel == channel && msg.length > pos){
		//Serial.print("hasData: ");
		//Serial.println(msg.hasData);
		//Serial.print("-length: ");
		//Serial.println(msg.length);
		return msg.length-pos;	
	}else if(listenForIncomingMessage(timeoutMillis,from).hasData){
		//Serial.println("hasDataDopo: ");
	    //Serial.println(msg.hasData);
		//Serial.println("Ricaricato");
		//Serial.print("pos2: ");
	    //Serial.print(pos);
		//Serial.print("-length2: ");
		//Serial.println(msg.length);
		return msg.length;
	}  
	return 0;   
}

bool ESP8266wifi::registerUDP(const char* remoteAddr, const char* remotePort, char* localPort, char channel){
    //rx_empty();
    writeCommand(CIPSTART_2);
    _serialOut->print(channel);
    writeCommand(COMMA_3);
    writeCommand(UDP);
    writeCommand(COMMA_2);
    _serialOut->print(remoteAddr);
    writeCommand(COMMA_1);
    _serialOut->print(remotePort);
    writeCommand(COMMA);
    _serialOut->print(localPort);
    writeCommand(TWO_COMMA,EOL);
    
    flags.localServerRunning = (readCommand(5000, OK, NO_CHANGE) > 0);
    return flags.localServerRunning;
}

bool ESP8266wifi::beginUDPServer(char* localPort, char channel){
    return registerUDP("0","0",localPort,channel);
}

/*
bool ESP8266wifi::beginUDPServer( char* localPort, char channel){
    writeCommand(CIPSTART_2);
    _serialOut->print(channel);+
    writeCommand(UDPNULL);
    _serialOut->print(localPort);
    writeCommand(TWO_COMMA,EOF);
    
    flags.localServerRunning = (readCommand(5000, OK, NO_CHANGE) > 0);
    return flags.localServerRunning;
}*/
