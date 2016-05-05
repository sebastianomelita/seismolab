// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "seismometer.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif
// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
#define OUTPUT_READABLE_ACCELGYRO

// uncomment "OUTPUT_BINARY_ACCELGYRO" to send all 6 axes of data as 16-bit
// binary, one right after the other. This is very fast (as fast as possible
// without compression or data loss), and easy to parse, but impossible to read
// for a human.
//#define OUTPUT_BINARY_ACCELGYRO

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
//#include <SoftwareSerial.h>

MPU6050 accelero;
statistics stat;

void seismometerInit() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
   #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
   #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
   #endif
  
  // initialize device
  Serial.println("Initializing I2C devices...");
  accelero.initialize();
  
  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelero.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
 
  //accelero.calibration();
  //accelero.MCUCalibration();
  //accelero.setAveraging(10);

  Serial.println();
}

void seismometerTick() {
	RECORD db = {0, 0, false};
	double detectionAVG = stat.getCurrentAVG();
	double detectionStdDev = stat.getCurrentSTDDEV();
  
	//db.ts = getUNIXTime();
	db.accel = stat.xyztomod(accelero.getAccelerationX(),accelero.getAccelerationY(),accelero.getAccelerationZ());
	db.overThreshold = db.accel > stat.getQuakeThreshold();
    stat.addValueToAvgVar(db.accel);
	
	/*
	if (inEvent && (millis() - lastEventWas >= 5000)) {
		// Out of event
		LED::red(false);
		inEvent = false;
		Serial.println("QUAKE Timeout END");
	} else if (inEvent && (millis() - lastEventWas) < 5000) {
		// In event, skipping detections for 5 seconds
		return;
	}

	// if the values of the accelerometer have passed the threshold
	//  or if an "event" is currently running
	//if (db.overThreshold && !inEvent) {
	if (db.overThreshold && !inEvent) {
		//Log::i("New Event: v:%lf - thr:%f - iter:%f - avg:%f - stddev:%f", db.accel, quakeThreshold,
			   //stat.getSigmaIter(), stat.detectionAVG, stat.detectionStdDev);

		LED::red(true);

		inEvent = true;
		lastEventWas = millis();

		//HTTPClient::httpSendAlert(&db);
		Serial.print("QUAKE: ");
		Serial.println(db.accel);
		httpQuakeRequest();
	}*/
/*
	// TODO: better checking
	if(db.overThreshold) {
		LED::red(true);
		// QUAKE
		Serial.print("QUAKE: ");
		Serial.println(db.accel);
		httpQuakeRequest();
		delay(5000);  
		Serial.println("QUAKE Timeout END");
		LED::red(false);
	}*/
}

