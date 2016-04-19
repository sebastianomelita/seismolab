# How to setup

Basically two ways are available to getting started with the NodeMCU devkit and the Arduino Environment. Right now we recommend the use of the git version, because crucial improvements have not been merged yet in the 2.0.0 stable version.

### Installing with Boards Manager ###

Starting with 1.6.4, Arduino allows installation of third-party platform packages using Boards Manager. The esp8266 has packages available for Windows, Mac OS, and Linux (32 and 64 bit).

- Install Arduino 1.6.5 or 1.6.6 from the [Arduino website](http://www.arduino.cc/en/main/software).
- Start Arduino and open Preferences window.
- Enter ```http://arduino.esp8266.com/stable/package_esp8266com_index.json``` into *Additional Board Manager URLs* field. You can add multiple URLs, separating them with commas.
- Open Boards Manager from Tools > Board menu and install *esp8266* platform (and don't forget to select your ESP8266 board from Tools > Board menu after installation).

The best place to ask questions related to this core is ESP8266 community forum: http://www.esp8266.com/arduino.

### Using esp8266/Arduino git version

The esp8266/Arduino IDE 

- Install Arduino 1.6.5 or 1.6.6
- Go to Arduino directory
- Clone this repository into hardware/esp8266com/esp8266 directory (or clone it elsewhere and create a symlink)
```bash
cd hardware
mkdir esp8266com
cd esp8266com
git clone https://github.com/esp8266/Arduino.git esp8266
```
- Download binary tools (you need Python 2.7)
```bash
cd esp8266/tools
python get.py
```
- Restart Arduino


## External Dependencies

- [WiFiManager](https://github.com/tzapu/WiFiManager/)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)

## NodeJS Test Server

In the *NodeJS test server* directory you will find a simple Node.js web server accepting POST requests. This is useful and recommended for developing stages to test your http requests. So just make sure your script works reliably before using the production API and do a final deploy.

## Hardware Prototype

ITA - [Check out](http://www.hackerstribe.com/2016/costruire-un-sismometro-digitale-con-esp8266-nodemcu-e-mpu-6050/) an example of hardware prototype where this code is now running on.
