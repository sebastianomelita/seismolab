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
//statistics stat(0.20 / 32768.0); //scale factor
statistics stat((double)2.0/ 32768.0,5.0); //scale factor

double getCurrentAVG(){
	return stat.getCurrentAVG();
}

double getCurrentSTDDEV(){
	return stat.getCurrentSTDDEV();
}

void resetStat(){
stat.resetLastPeriod();  	
}

void seismometerInit() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
   #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
   #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
   #endif

  // initialize device
  Serial.println(F("Initializing I2C devices..."));
  accelero.initialize();
  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(accelero.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
  
  accelero.setFullScaleAccelRange(0); //+-2G
  Serial.print(F("Scale range: "));
  Serial.println(accelero.getFullScaleAccelRange());
  //accelero.calibration();
  //accelero.MCUCalibration();
  //accelero.setAveraging(10);
    //Serial.print(accelero.getXAccelOffset()); Serial.print("\t"); // -76
    //Serial.print(accelero.getYAccelOffset()); Serial.print("\t"); // -2359
    //Serial.print(accelero.getZAccelOffset()); Serial.print("\t"); // 1688
    //Serial.print("\n");
    accelero.setXAccelOffset(-2380);
    accelero.setYAccelOffset(500);
    accelero.setZAccelOffset(1810);
    //Serial.print(accelero.getXAccelOffset()); Serial.print("\t"); // -76
    //Serial.print(accelero.getYAccelOffset()); Serial.print("\t"); // -2359
    //Serial.print(accelero.getZAccelOffset()); Serial.print("\t"); // 1688

    Serial.print("\n");

  Serial.println();
}

void seismometerTick() {
	RECORD db = {0, 0, false};
	double detectionAVG = stat.getCurrentAVG();
	double detectionStdDev = stat.getCurrentSTDDEV();
    stat.setSigmaIter(getSigma());
    
	//db.ts = getUNIXTime();
	stat.setXYZ(accelero.getAccelerationX(),accelero.getAccelerationY(),accelero.getAccelerationZ());
	db.accel = stat.getModule();
	db.overThreshold = stat.getModuleEMA(0.6) > stat.getQuakeThreshold();
    stat.addValueToAvgVar(db.accel);
    
    //Serial.print(F("\ndb.accel: "));
     //Serial.print(db.accel);
    //Serial.println(stat.getQuakeThreshold());
    //Serial.print(F("-"));
    //Serial.print(stat.getModuleEMA(0.9));
    //Serial.print(F("-"));
	//Serial.println(stat.getQuakeThreshold());
	//Serial.println(accelero.getAccelerationY(),DEC);
	//Serial.println(accelero.getAccelerationZ(),DEC);

    
    //Serial.println(F("QUAKE: "));
    //Serial.println(db.overThreshold);
	
	// TODO: better checking
	//if(db.overThreshold) {
	if(db.overThreshold) {
		LED::red(true);
		// QUAKE
		Serial.println(F(""));
		Serial.print(F("QUAKE: "));
		httpQuakeRequest();
		Serial.print(db.accel);
		stat.resetLastPeriod();  
		accelero.resetSensors();
		accelero.resetFIFO();
		Serial.print(F("-"));
		//stat.getModuleEMA(0.6);
		Serial.print(F("-"));
		Serial.println(stat.getModuleEMA(0.6));
		delay(5000);
		//accelero.setStandbyXAccelEnabled(false);
		//accelero.setStandbyYAccelEnabled(false);
		//accelero.setStandbyZAccelEnabled(false);
		Serial.println(F("QUAKE Timeout END"));
		LED::red(false);
	}
}


