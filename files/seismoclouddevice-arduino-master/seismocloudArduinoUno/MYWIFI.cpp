#include "MYWIFI.h"
#include <Arduino.h>

SoftwareSerial *MYWIFI::_uart;
uint32_t MYWIFI::_port;

ESP8266 & MYWIFI::getWifi(){ 
		return getWifi(*_uart, _port);
}
