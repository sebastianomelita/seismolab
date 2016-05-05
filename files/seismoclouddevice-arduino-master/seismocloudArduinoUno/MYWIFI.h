//
// Created by enrico on 09/07/15.
//

#ifndef __MYWIFI_H
#define __MYWIFI_H

#include <stdint.h>
#include "ITEADLIB/ESP8266.h"
#include "SoftwareSerial.h"

class MYWIFI{
public:
	static ESP8266 &getWifi(SoftwareSerial &uart, uint32_t port){
		_uart=&uart;
		_port=port;	
		static ESP8266 wifi(uart,port);  
		return wifi;
	}

    static ESP8266 &getWifi();

private:
    static SoftwareSerial *_uart;
	static uint32_t _port;	
	    // ecco il costruttore privato in modo che l'utente non possa istanziare direttamente
	MYWIFI() { };
	    // il costruttore di copia va solo dichiarato, non definito!
	MYWIFI(const MYWIFI&);
	    // stessa cosa per l'operatore di assegnamento: solo dichiarato, non definito!
    void operator=(const MYWIFI&);
  
};

#endif //MYWIFI_H

