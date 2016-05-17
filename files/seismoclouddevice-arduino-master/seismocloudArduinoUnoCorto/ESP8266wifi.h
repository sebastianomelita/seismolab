//
//  ESP8266wifi.h
//
//
//  Created by Jonas Ekstrand on 2015-02-20.
//  ESP8266 AT cmd ref from https://github.com/espressif/esp8266_at/wiki/CIPSERVER
//
//

#ifndef ESP8266wifi_h
#define ESP8266wifi_h

#define HW_RESET_RETRIES 3
#define SERVER_CONNECT_RETRIES_BEFORE_HW_RESET 3

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <inttypes.h>
#include <avr/pgmspace.h>
#include "HardwareSerial.h"

#define SERVER '4'
#define FIRST '0'
#define MAX_CONNECTIONS 3

#define MSG_BUFFER_MAX 128
/*
struct WifiMessage{
public:
    bool hasData:1;
    char channel;
    char * message;
};*/
struct WifiMessage{
public:
    bool hasData:1;
    char channel;
    char * message;
    byte length;
    bool first;
};


struct WifiConnection{
public:
    char channel;
    bool connected:1;
};

struct Flags   // 1 byte value (on a system where 8 bits is a byte
{
    bool started:1, 
         echoOnOff:1, 
         debug:1, 
         serverConfigured:1,            // true if a connection to a remote server is configured
         connectedToServer:1,           // true if a connection to a remote server is established
         apConfigured:1,                // true if the module is configured as a client station
         localApConfigured:1,
         localServerConfigured:1,
         localApRunning:1, 
         localServerRunning:1, 
         endSendWithNewline:1, 
         connectToServerUsingTCP:1,
         transparentMode:1;
};

class ESP8266wifi
{
    
public:
    /*
     * Will pull resetPin low then high to reset esp8266, connect this pin to CHPD pin
     */
    ESP8266wifi(Stream &serialIn, Stream &serialOut, byte resetPin);
    
    
    /*
     * Will pull resetPin low then high to reset esp8266, connect this pin to CHPD pin
     */
    ESP8266wifi(Stream &serialIn, Stream &serialOut, byte resetPin, Stream &dbgSerial);
    
    /*
     * Will do hw reset and set inital configuration, will try this HW_RESET_RETRIES times.
     */
    bool begin(); // reset and set echo and other stuff
    
    bool isStarted();
    
    /*
     * Connect to AP using wpa encryption
     * (reconnect logic is applied, if conn lost or not established, or esp8266 restarted)
     */
    bool connectToAP(String& ssid, String& password);
    bool connectToAP(const char* ssid, const char* password);
    bool isConnectedToAP();
    char* getIP();
    char* getMAC();
    char* getVersion();
    
    /*
     * Connecting with TCP to server
     * (reconnect logic is applied, if conn lost or not established, or esp8266 restarted)
     */
    
    void setTransportToUDP();
    //Default..
    void setTransportToTCP();
    bool connectToServer(String& ip, String& port, bool peerChange=true);
    bool connectToServer(const char* ip, const char* port, bool peerChange=true);
    void disconnectFromServer();
    bool isConnectedToServer();
    
    /*
     * Starting local AP and local TCP-server
     * (reconnect logic is applied, if conn lost or not established, or esp8266 restarted)
     */
    bool startLocalAPAndServer(const char* ssid, const char* password, const char* channel,const char* port);
    bool startLocalAP(const char* ssid, const char* password, const char* channel);
    bool startLocalServer(const char* port);
    bool stopLocalAPAndServer();
    bool stopLocalAP();
    bool stopLocalServer();
    bool isLocalAPAndServerRunning();
    
    
    /*
     * Send string (if channel is connected of course)
     */
    bool send(char channel, String& message, bool sendNow = false);
    bool send(char channel, const char * message, bool sendNow = false);
    
    /*
     * Default is true.
     */
    void endSendWithNewline(bool endSendWithNewline);
    
    /*
     * Scan for incoming message, do this as often and as long as you can (use as sleep in loop)
     */
    WifiMessage listenForIncomingMessage(int timeoutMillis, char *from=NULL);
    WifiMessage getIncomingMessage(char *from=NULL);
    bool isConnection(void);
    bool checkConnections(WifiConnection **pConnections);
    
    //--------------MODIFICHE ALLA LIBRERIA ORIGINALE--------------------------------------
    bool startNTPClient();
    bool stopNTPClient();
    char* getNTP();
    bool startTransparentMode();
    bool stopTransparentMode();
	//Per connessioni UDP------------------------------------------------------------------
	bool registerUDP(char* addr, char* port, char channel='0');
    bool beginUDPPacket(const char* host, const char* port, bool transparent=false); //connette ad un server UDP imposta writeChannel=SERVER
    bool beginTCPConnection(const char* host, const char* port); //connette ad un server TCP imposta writeChannel=SERVER
    bool beginUDPPacket(char channel);
    //bool beginLocalServer(const char* port); //fa partire un server in attesa su tutte le connessioni (channel)
    bool endUDPPacket(char channel=SERVER);
    int parseUDPPacket(int timeout=10, char *from=NULL);
    unsigned char write(const unsigned char buf);
    int write(const unsigned char* buf, size_t size);
    char read();
    size_t read(char* buf, size_t size);
    char getCurrLinkId();
    int available(int timeoutMillis=10, char *from=NULL, char channel=SERVER);
    bool beginUDPServer(char* localPort, char channel= FIRST);
    bool registerUDP(const char* remoteAddr, const char* remotePort, char* localPort, char channel=FIRST);
    //per connesioni TCP-------------------
    void print(char *s, char channel=SERVER);
	void println(char *s, char channel=SERVER);
	void println(char channel=SERVER);
    void print(const __FlashStringHelper *s, char channel=SERVER);
	void println(const __FlashStringHelper *s, char channel=SERVER);
    char readTCP(char *from=NULL);  //legge un carattere  ue eventualmente ricarica da una qualunque cannessione
    int readLine(char* buf, size_t bufmax); //legge dal buffer di ingresso una linea senza ricaricare
    //metodo di classe singleton da invocare alla prima chiamata
    static ESP8266wifi &getWifi(Stream &serialIn, Stream &serialOut, byte resetPin, Stream &dbgSerial){
    	// l'unica istanza della classe viene creata alla prima chiamata di getWifi()
        // e verr� distrutta solo all'uscita dal programma
		static ESP8266wifi wifi(serialIn,serialOut,resetPin,dbgSerial); 
		return wifi;
	}

    static ESP8266wifi &getWifi();

private:
	//modificate rispetto all'originale
	//sono adesso propriet� di classe
    static Stream* _serialIn;
    static Stream* _serialOut;
    static byte _resetPin;
    static Stream* _dbgSerial;
    //-----fine propriet� di classe-------------
    //in pi� rispetto all'originale-------------
    static WifiMessage msg;
    uint16_t  pos;  //segnaposto
    uint16_t  posw;  //segnaposto
    uint16_t  offset; //spiazzamento dal punto di inizio di copia del messaggio 
    //fine varianti-------------------
    
    Flags flags;
    
    bool connectToServer(bool peerChange=true);
    char _ip[30];
    char _port[6];
    
    bool connectToAP();
    char _ssid[16];
    char _password[20];
    
    bool startLocalAp();
    bool startLocalServer();
    char _localAPSSID[16];
    char _localAPPassword[16];
    char _localAPChannel[3];
    char _localServerPort[6];
    WifiConnection _connections[MAX_CONNECTIONS];
    
    bool restart();
    
    byte serverRetries;
    bool watchdog();
    
    char msgOut[MSG_BUFFER_MAX];//buffer for send method
    char msgIn[MSG_BUFFER_MAX]; //buffer for listen method = limit of incoming message..

    void writeCommand(const char* text1, const char* text2 = NULL);
    byte readCommand(int timeout, const char* text1 = NULL, const char* text2 = NULL);
    //byte readCommand(const char* text1, const char* text2);
    uint16_t readBuffer(char* buf, uint16_t count, char delim = '\0');
    uint16_t readBuffer2(char* buf, uint16_t count);
    char readChar();
    //----------aggiunte-----------------------
    uint16_t getFrom(char *from);
    void rxEmpty();


    //Stream* _dbgSerial;
   
    //--------------MODIFICHE ALLA LIBRERIA ORIGINALE--------------------------------------
        // ecco il costruttore privato in modo che l'utente non possa istanziare direttamente
	ESP8266wifi() { };
	    // il costruttore di copia va solo dichiarato, non definito!
	ESP8266wifi(const ESP8266wifi&);
	    // stessa cosa per l'operatore di assegnamento: solo dichiarato, non definito!
    void operator=(const ESP8266wifi&);
    
};

#endif
