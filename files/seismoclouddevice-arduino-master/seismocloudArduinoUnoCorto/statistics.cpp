#include "statistics.h"

statistics::statistics(double scaleMultiplier,int sumxx,int sumyy,int sumzz){
	scalef=scaleMultiplier;
	sumx=sumxx;
	sumy=sumyy;
	sumz=sumzz;
	ex=ey=ez=0;
}

// getModule returns the magnitude of the total acceleration vector as an integer
double statistics::xyztomod(int xx, int yy, int zz)
{
  return sqrt(square(xx*scalef+sumx) + square(yy*scalef+sumy) + square(zz*scalef+sumz));
}

void statistics::setXYZ(int cx, int cy, int cz){
	x=cx*scalef+sumx;
	y=cy*scalef+sumy;
	z=cz*scalef+sumz;
}
/*
void statistics::setFactor(double f){
	factor=f;
}
*/
// getModule returns the magnitude of the total acceleration vector as an integer
double statistics::getModule(int x, int y, int z)
{
  return sqrt(square(x) + square(y) + square(z));
}

/*int statistics::getCalibratedModule()
{
  //return sqrt(square(_mapMPU6050G(getAccelerationX()*AccelerationFactor-iX)) + square(_mapMPU6050G(getAccelerationX()*AccelerationFactor-iY)) + square(_mapMPU6050G(getAccelerationZ()*AccelerationFactor-iZ)));
  return sqrt(square(x-iX) + square(y-iY) + square(z-iZ));
}

int statistics::getCalibratedModule(int x, int y, int z)
{
  setXYZ(x,y,z);
  return getCalibratedModule();
}

int statistics::getCalibratedModuleMCU()
{
  MCUx=MCUx-MCUx/n + x/n;
  MCUy=MCUy-MCUy/n + y/n;
  MCUz=MCUz-MCUz/n + z/n;
  n++;
  return sqrt(square(x-iX) + square(y-iY) + square(z-iZ));
}

int statistics::getCalibratedModuleMCU(int x, int y, int z)
{
  setXYZ(x,y,z);
  return getCalibratedModuleMCU();
}

int statistics::getCalibratedModuleMCUEMA(float a)
{
  MCUx=MCUx-MCUx/n + x/n;
  MCUy=MCUy-MCUy/n + y/n;
  MCUz=MCUz-MCUz/n + z/n;
  n++;
  ex=x*a+(1-a)*ex;
  ey=y*a+(1-a)*ey;
  ez=z*a+(1-a)*ez;
  return sqrt(square(ex*factor-iX) + square(ey*factor-iY) + square(ez*factor-iZ));
}
*/
double statistics::getModuleEMA(double a)
{
  ex=(double) x*a+(1.0-a)*ex;
  ey=(double) y*a+(1.0-a)*ey;
  ez=(double) z*a+(1.0-a)*ez;
  return sqrt(square(ex) + square(ey) + square(ez));
}

/*
int statistics::getCalibratedModuleMCUEMA(float a, int x, int y, int z)
{
  setXYZ(x,y,z);
  return getCalibratedModuleMCUEMA(a);
}

void statistics::calibration(){  
  iX=x;  
  iY=y;
  iZ=z;
}

void statistics::MCUCalibration(){    
  //valori di riferimento temporanei
  iX = MCUx;  
  iY = MCUy;
  iZ = MCUz;
  MCUx=0;
  MCUy=0;
  MCUz=0;
  ex=x; 
  ey=y; 
  ez=z; 
}
*/
double statistics::getQuakeThreshold() {
	return quakeThreshold;
}

void statistics::setSigmaIter(double i) {
	this->sigmaIter = i;
}

void statistics::addValueToAvgVar(double val) {
	elements++;
	// https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
	double delta = val - partialAvg;
	partialAvg += delta / elements;
	partialStdDev += delta * (val - partialAvg);
	if (elements > 1) {
		quakeThreshold = partialAvg + (getCurrentSTDDEV() * getSigmaIter());
	}
	//Log::d("AddValueToAvgVar: EL:%f D:%f AVG:%f VAR:%f THR:%f I:%i", val, delta, getCurrentAVG(), getCurrentSTDDEV(),
	//	   quakeThreshold, elements);
}

void statistics::addValueToAvgVar(int x, int y, int z)
{  
	addValueToAvgVar(xyztomod(x,y,z));
}

void statistics::resetLastPeriod() {
	partialAvg = 0;
	partialStdDev = 0;
	elements = 0;
	ex=ey=ez=0;
}

double statistics::getSigmaIter() {
	return sigmaIter;
}

double statistics::getCurrentAVG() {
	return partialAvg;
}

double statistics::getCurrentSTDDEV() {
	return sqrt(partialStdDev / (elements - 1));
}

