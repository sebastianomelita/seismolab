#ifndef __statistics_h
#define __statistics_h

#include "Arduino.h"
class statistics {
    public:
        statistics();
		double xyztomod(int,int,int);
		double getModule();
		double getModule(int,int,int);
		//int getCalibratedModule();
		//int getCalibratedModule(int,int,int);
		//int getCalibratedModuleMCU();
		//int getCalibratedModuleMCU(int,int,int);
		//int getCalibratedModuleMCUEMA(float);
		//int getCalibratedModuleMCUEMA(float,int,int,int);
		//void calibration();
		//void MCUCalibration();
		void setXYZ(int,int,int);
		unsigned int getStatProbeSpeed();
		double getQuakeThreshold();
		double getCurrentAVG();
		double getCurrentSTDDEV();
		void setSigmaIter(double);
		double getSigmaIter();
		void resetLastPeriod();
		void addValueToAvgVar(double);
		void addValueToAvgVar(int, int, int );
		//void setFactor(double f);
     private:
        uint8_t devAddr;
		//int16_t IX,IY,IZ,AcX,AcY,AcZ,Tmp, x, y, z;
		int16_t x, y, z;
		//double iX, iY, iZ, X, Y, Z;
		//int16_t ex,ey,ez;
		//int MCUx,MCUy,MCUz;
		double partialAvg = 0;
	    double partialStdDev = 0;
	    unsigned int elements = 0;
	    double quakeThreshold = 1;
	    double sigmaIter = 3;
	    //double factor;
};
#endif
