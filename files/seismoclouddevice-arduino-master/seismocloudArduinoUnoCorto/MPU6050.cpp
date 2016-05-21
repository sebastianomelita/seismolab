// I2Cdev library collection - MPU6050 I2C device class
// Based on InvenSense MPU-6050 register map document rev. 2.0, 5/19/2011 (RM-MPU-6000A-00)
// 8/24/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     ... - ongoing debug release

// NOTE: THIS IS ONLY A PARIAL RELEASE. THIS DEVICE CLASS IS CURRENTLY UNDERGOING ACTIVE
// DEVELOPMENT AND IS STILL MISSING SOME IMPORTANT FEATURES. PLEASE KEEP THIS IN MIND IF
// YOU DECIDE TO USE THIS PARTICULAR CODE FOR ANYTHING.

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

#include "MPU6050.h"

/** Default constructor, uses default I2C address.
 * @see MPU6050_DEFAULT_ADDRESS
 */
MPU6050::MPU6050() {
    devAddr = MPU6050_DEFAULT_ADDRESS;
}

/** Specific address constructor.
 * @param address I2C address
 * @see MPU6050_DEFAULT_ADDRESS
 * @see MPU6050_ADDRESS_AD0_LOW
 * @see MPU6050_ADDRESS_AD0_HIGH
 */
MPU6050::MPU6050(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 * This will activate the device and take it out of sleep mode (which must be done
 * after start-up). This function also sets both the accelerometer and the gyroscope
 * to their most sensitive settings, namely +/- 2g and +/- 250 degrees/sec, and sets
 * the clock source to use the X Gyro for reference, which is slightly better than
 * the default internal clock source.
 */
void MPU6050::initialize() {
    setClockSource(MPU6050_CLOCK_PLL_XGYRO);
    setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
    setSleepEnabled(false); // thanks to Jack Elston for pointing this one out!
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool MPU6050::testConnection() {
    return getDeviceID() == 0x34;
}


uint8_t MPU6050::getDLPFMode() {
    I2Cdev::readBits(devAddr, MPU6050_RA_CONFIG, MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, buffer);
    return buffer[0];
}
/** Set digital low-pass filter configuration.
 * @param mode New DLFP configuration setting
 * @see getDLPFBandwidth()
 * @see MPU6050_DLPF_BW_256
 * @see MPU6050_RA_CONFIG
 * @see MPU6050_CFG_DLPF_CFG_BIT
 * @see MPU6050_CFG_DLPF_CFG_LENGTH
 */
void MPU6050::setDLPFMode(uint8_t mode) {
    I2Cdev::writeBits(devAddr, MPU6050_RA_CONFIG, MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, mode);
}

// GYRO_CONFIG register

/** Get full-scale gyroscope range.
 * The FS_SEL parameter allows setting the full-scale range of the gyro sensors,
 * as described in the table below.
 *
 * <pre>
 * 0 = +/- 250 degrees/sec
 * 1 = +/- 500 degrees/sec
 * 2 = +/- 1000 degrees/sec
 * 3 = +/- 2000 degrees/sec
 * </pre>
 *
 * @return Current full-scale gyroscope range setting
 * @see MPU6050_GYRO_FS_250
 * @see MPU6050_RA_GYRO_CONFIG
 * @see MPU6050_GCONFIG_FS_SEL_BIT
 * @see MPU6050_GCONFIG_FS_SEL_LENGTH
 /
uint8_t MPU6050::getFullScaleGyroRange() {
    I2Cdev::readBits(devAddr, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, buffer);
    return buffer[0];
}
/** Set full-scale gyroscope range.
 * @param range New full-scale gyroscope range value
 * @see getFullScaleRange()
 * @see MPU6050_GYRO_FS_250
 * @see MPU6050_RA_GYRO_CONFIG
 * @see MPU6050_GCONFIG_FS_SEL_BIT
 * @see MPU6050_GCONFIG_FS_SEL_LENGTH
 /
void MPU6050::setFullScaleGyroRange(uint8_t range) {
    I2Cdev::writeBits(devAddr, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}

// SELF TEST FACTORY TRIM VALUES

/** Get self-test factory trim value for accelerometer X axis.
 * @return factory trim value
 * @see MPU6050_RA_SELF_TEST_X
 */
uint8_t MPU6050::getAccelXSelfTestFactoryTrim() {
    I2Cdev::readByte(devAddr, MPU6050_RA_SELF_TEST_X, &buffer[0]);
	I2Cdev::readByte(devAddr, MPU6050_RA_SELF_TEST_A, &buffer[1]);	
    return (buffer[0]>>3) | ((buffer[1]>>4) & 0x03);
}

/** Get self-test factory trim value for accelerometer Y axis.
 * @return factory trim value
 * @see MPU6050_RA_SELF_TEST_Y
 */
uint8_t MPU6050::getAccelYSelfTestFactoryTrim() {
    I2Cdev::readByte(devAddr, MPU6050_RA_SELF_TEST_Y, &buffer[0]);
	I2Cdev::readByte(devAddr, MPU6050_RA_SELF_TEST_A, &buffer[1]);	
    return (buffer[0]>>3) | ((buffer[1]>>2) & 0x03);
}

/** Get self-test factory trim value for accelerometer Z axis.
 * @return factory trim value
 * @see MPU6050_RA_SELF_TEST_Z
 */
uint8_t MPU6050::getAccelZSelfTestFactoryTrim() {
    I2Cdev::readBytes(devAddr, MPU6050_RA_SELF_TEST_Z, 2, buffer);	
    return (buffer[0]>>3) | (buffer[1] & 0x03);
}

/** Get self-test enabled setting for accelerometer X axis.
 * @return Self-test enabled value
 * @see MPU6050_RA_ACCEL_CONFIG
 */
bool MPU6050::getAccelXSelfTest() {
    I2Cdev::readBit(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_XA_ST_BIT, buffer);
    return buffer[0];
}
/** Get self-test enabled setting for accelerometer X axis.
 * @param enabled Self-test enabled value
 * @see MPU6050_RA_ACCEL_CONFIG
 */
void MPU6050::setAccelXSelfTest(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_XA_ST_BIT, enabled);
}
/** Get self-test enabled value for accelerometer Y axis.
 * @return Self-test enabled value
 * @see MPU6050_RA_ACCEL_CONFIG
 */
bool MPU6050::getAccelYSelfTest() {
    I2Cdev::readBit(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_YA_ST_BIT, buffer);
    return buffer[0];
}
/** Get self-test enabled value for accelerometer Y axis.
 * @param enabled Self-test enabled value
 * @see MPU6050_RA_ACCEL_CONFIG
 */
void MPU6050::setAccelYSelfTest(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_YA_ST_BIT, enabled);
}
/** Get self-test enabled value for accelerometer Z axis.
 * @return Self-test enabled value
 * @see MPU6050_RA_ACCEL_CONFIG
 */
bool MPU6050::getAccelZSelfTest() {
    I2Cdev::readBit(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_ZA_ST_BIT, buffer);
    return buffer[0];
}
/** Set self-test enabled value for accelerometer Z axis.
 * @param enabled Self-test enabled value
 * @see MPU6050_RA_ACCEL_CONFIG
 */
void MPU6050::setAccelZSelfTest(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_ZA_ST_BIT, enabled);
}
/** Get full-scale accelerometer range.
 * The FS_SEL parameter allows setting the full-scale range of the accelerometer
 * sensors, as described in the table below.
 *
 * <pre>
 * 0 = +/- 2g
 * 1 = +/- 4g
 * 2 = +/- 8g
 * 3 = +/- 16g
 * </pre>
 *
 * @return Current full-scale accelerometer range setting
 * @see MPU6050_ACCEL_FS_2
 * @see MPU6050_RA_ACCEL_CONFIG
 * @see MPU6050_ACONFIG_AFS_SEL_BIT
 * @see MPU6050_ACONFIG_AFS_SEL_LENGTH
 */
uint8_t MPU6050::getFullScaleAccelRange() {
    I2Cdev::readBits(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, buffer);
    return buffer[0];
}
/** Set full-scale accelerometer range.
 * @param range New full-scale accelerometer range setting
 * @see getFullScaleAccelRange()
 */
void MPU6050::setFullScaleAccelRange(uint8_t range) {
    I2Cdev::writeBits(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}
/** Get accelerometer FIFO enabled value.
 * When set to 1, this bit enables ACCEL_XOUT_H, ACCEL_XOUT_L, ACCEL_YOUT_H,
 * ACCEL_YOUT_L, ACCEL_ZOUT_H, and ACCEL_ZOUT_L (Registers 59 to 64) to be
 * written into the FIFO buffer.
 * @return Current accelerometer FIFO enabled value
 * @see MPU6050_RA_FIFO_EN
 */
bool MPU6050::getAccelFIFOEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_FIFO_EN, MPU6050_ACCEL_FIFO_EN_BIT, buffer);
    return buffer[0];
}
/** Set accelerometer FIFO enabled value.
 * @param enabled New accelerometer FIFO enabled value
 * @see getAccelFIFOEnabled()
 * @see MPU6050_RA_FIFO_EN
 */
void MPU6050::setAccelFIFOEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_FIFO_EN, MPU6050_ACCEL_FIFO_EN_BIT, enabled);
}
/** Get I2C master clock speed.
 * I2C_MST_CLK is a 4 bit unsigned value which configures a divider on the
 * MPU-60X0 internal 8MHz clock. It sets the I2C master clock speed according to
 * the following table:
 *
 * <pre>
 * I2C_MST_CLK | I2C Master Clock Speed | 8MHz Clock Divider
 * ------------+------------------------+-------------------
 * 0           | 348kHz                 | 23
 * 1           | 333kHz                 | 24
 * 2           | 320kHz                 | 25
 * 3           | 308kHz                 | 26
 * 4           | 296kHz                 | 27
 * 5           | 286kHz                 | 28
 * 6           | 276kHz                 | 29
 * 7           | 267kHz                 | 30
 * 8           | 258kHz                 | 31
 * 9           | 500kHz                 | 16
 * 10          | 471kHz                 | 17
 * 11          | 444kHz                 | 18
 * 12          | 421kHz                 | 19
 * 13          | 400kHz                 | 20
 * 14          | 381kHz                 | 21
 * 15          | 364kHz                 | 22
 * </pre>
 *
 * @return Current I2C master clock speed
 * @see MPU6050_RA_I2C_MST_CTRL
 */
uint8_t MPU6050::getMasterClockSpeed() {
    I2Cdev::readBits(devAddr, MPU6050_RA_I2C_MST_CTRL, MPU6050_I2C_MST_CLK_BIT, MPU6050_I2C_MST_CLK_LENGTH, buffer);
    return buffer[0];
}
/** Set I2C master clock speed.
 * @reparam speed Current I2C master clock speed
 * @see MPU6050_RA_I2C_MST_CTRL
 */
void MPU6050::setMasterClockSpeed(uint8_t speed) {
    I2Cdev::writeBits(devAddr, MPU6050_RA_I2C_MST_CTRL, MPU6050_I2C_MST_CLK_BIT, MPU6050_I2C_MST_CLK_LENGTH, speed);
}

void MPU6050::getAcceleration(int16_t* x, int16_t* y, int16_t* z) {
    I2Cdev::readBytes(devAddr, MPU6050_RA_ACCEL_XOUT_H, 6, buffer);
    *x = (((int16_t)buffer[0]) << 8) | buffer[1];
    *y = (((int16_t)buffer[2]) << 8) | buffer[3];
    *z = (((int16_t)buffer[4]) << 8) | buffer[5];
}
/** Get X-axis accelerometer reading.
 * @return X-axis acceleration measurement in 16-bit 2's complement format
 * @see getMotion6()
 * @see MPU6050_RA_ACCEL_XOUT_H
 */
int16_t MPU6050::getAccelerationX() {
    I2Cdev::readBytes(devAddr, MPU6050_RA_ACCEL_XOUT_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
/** Get Y-axis accelerometer reading.
 * @return Y-axis acceleration measurement in 16-bit 2's complement format
 * @see getMotion6()
 * @see MPU6050_RA_ACCEL_YOUT_H
 */
int16_t MPU6050::getAccelerationY() {
    I2Cdev::readBytes(devAddr, MPU6050_RA_ACCEL_YOUT_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
/** Get Z-axis accelerometer reading.
 * @return Z-axis acceleration measurement in 16-bit 2's complement format
 * @see getMotion6()
 * @see MPU6050_RA_ACCEL_ZOUT_H
 */
int16_t MPU6050::getAccelerationZ() {
    I2Cdev::readBytes(devAddr, MPU6050_RA_ACCEL_ZOUT_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}

// TEMP_OUT_* registers

/** Get current internal temperature.
 * @return Temperature reading in 16-bit 2's complement format
 * @see MPU6050_RA_TEMP_OUT_H
 */
int16_t MPU6050::getTemperature() {
    I2Cdev::readBytes(devAddr, MPU6050_RA_TEMP_OUT_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
/** Reset temperature sensor signal path.
 * The reset will revert the signal path analog to digital converters and
 * filters to their power up configurations.
 * @see MPU6050_RA_SIGNAL_PATH_RESET
 * @see MPU6050_PATHRESET_TEMP_RESET_BIT
 
// USER_CTRL register

/** Get FIFO enabled status.
 * When this bit is set to 0, the FIFO buffer is disabled. The FIFO buffer
 * cannot be written to or read from while disabled. The FIFO buffer's state
 * does not change unless the MPU-60X0 is power cycled.
 * @return Current FIFO enabled status
 * @see MPU6050_RA_USER_CTRL
 * @see MPU6050_USERCTRL_FIFO_EN_BIT
 */
bool MPU6050::getFIFOEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_FIFO_EN_BIT, buffer);
    return buffer[0];
}
/** Set FIFO enabled status.
 * @param enabled New FIFO enabled status
 * @see getFIFOEnabled()
 * @see MPU6050_RA_USER_CTRL
 * @see MPU6050_USERCTRL_FIFO_EN_BIT
 */
void MPU6050::setFIFOEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_FIFO_EN_BIT, enabled);
}
/** Get I2C Master Mode enabled status.
 * When this mode is enabled, the MPU-60X0 acts as the I2C Master to the
 * external sensor slave devices on the auxiliary I2C bus. When this bit is
 * cleared to 0, the auxiliary I2C bus lines (AUX_DA and AUX_CL) are logically
 * driven by the primary I2C bus (SDA and SCL). This is a precondition to
 * enabling Bypass Mode. For further information regarding Bypass Mode, please
 * refer to Register 55.
 * @return Current I2C Master Mode enabled status
 * @see MPU6050_RA_USER_CTRL
 * @see MPU6050_USERCTRL_I2C_MST_EN_BIT
 */
bool MPU6050::getI2CMasterModeEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT, buffer);
    return buffer[0];
}
/** Set I2C Master Mode enabled status.
 * @param enabled New I2C Master Mode enabled status
 * @see getI2CMasterModeEnabled()
 * @see MPU6050_RA_USER_CTRL
 * @see MPU6050_USERCTRL_I2C_MST_EN_BIT
 */
void MPU6050::setI2CMasterModeEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT, enabled);
}
/** Switch from I2C to SPI mode (MPU-6000 only)
 * If this is set, the primary SPI interface will be enabled in place of the
 * disabled primary I2C interface.
 */
void MPU6050::switchSPIEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_IF_DIS_BIT, enabled);
}
/** Reset the FIFO.
 * This bit resets the FIFO buffer when set to 1 while FIFO_EN equals 0. This
 * bit automatically clears to 0 after the reset has been triggered.
 * @see MPU6050_RA_USER_CTRL
 * @see MPU6050_USERCTRL_FIFO_RESET_BIT
 */
void MPU6050::resetFIFO() {
    I2Cdev::writeBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_FIFO_RESET_BIT, true);
}
/** Reset the I2C Master.
 * This bit resets the I2C Master when set to 1 while I2C_MST_EN equals 0.
 * This bit automatically clears to 0 after the reset has been triggered.
 * @see MPU6050_RA_USER_CTRL
 * @see MPU6050_USERCTRL_I2C_MST_RESET_BIT
 */
void MPU6050::resetI2CMaster() {
    I2Cdev::writeBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_RESET_BIT, true);
}
/** Reset all sensor registers and signal paths.
 * When set to 1, this bit resets the signal paths for all sensors (gyroscopes,
 * accelerometers, and temperature sensor). This operation will also clear the
 * sensor registers. This bit automatically clears to 0 after the reset has been
 * triggered.
 *
 * When resetting only the signal path (and not the sensor registers), please
 * use Register 104, SIGNAL_PATH_RESET.
 *
 * @see MPU6050_RA_USER_CTRL
 * @see MPU6050_USERCTRL_SIG_COND_RESET_BIT
 */
void MPU6050::resetSensors() {
    I2Cdev::writeBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_SIG_COND_RESET_BIT, true);
}

// PWR_MGMT_1 register

/** Trigger a full device reset.
 * A small delay of ~50ms may be desirable after triggering a reset.
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_DEVICE_RESET_BIT
 */
void MPU6050::reset() {
    I2Cdev::writeBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_DEVICE_RESET_BIT, true);
}
/** Get sleep mode status.
 * Setting the SLEEP bit in the register puts the device into very low power
 * sleep mode. In this mode, only the serial interface and internal registers
 * remain active, allowing for a very low standby current. Clearing this bit
 * puts the device back into normal mode. To save power, the individual standby
 * selections for each of the gyros should be used if any gyro axis is not used
 * by the application.
 * @return Current sleep mode enabled status
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_SLEEP_BIT
 */
bool MPU6050::getSleepEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, buffer);
    return buffer[0];
}
/** Set sleep mode status.
 * @param enabled New sleep mode enabled status
 * @see getSleepEnabled()
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_SLEEP_BIT
 */
void MPU6050::setSleepEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled);
}
/** Get wake cycle enabled status.
 * When this bit is set to 1 and SLEEP is disabled, the MPU-60X0 will cycle
 * between sleep mode and waking up to take a single sample of data from active
 * sensors at a rate determined by LP_WAKE_CTRL (register 108).
 * @return Current sleep mode enabled status
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_CYCLE_BIT
 /
bool MPU6050::getWakeCycleEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CYCLE_BIT, buffer);
    return buffer[0];
}
/** Set wake cycle enabled status.
 * @param enabled New sleep mode enabled status
 * @see getWakeCycleEnabled()
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_CYCLE_BIT
 */
void MPU6050::setWakeCycleEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CYCLE_BIT, enabled);
}
/** Get temperature sensor enabled status.
 * Control the usage of the internal temperature sensor.
 *
 * Note: this register stores the *disabled* value, but for consistency with the
 * rest of the code, the function is named and used with standard true/false
 * values to indicate whether the sensor is enabled or disabled, respectively.
 *
 * @return Current temperature sensor enabled status
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_TEMP_DIS_BIT
 */
bool MPU6050::getTempSensorEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_TEMP_DIS_BIT, buffer);
    return buffer[0] == 0; // 1 is actually disabled here
}
/** Set temperature sensor enabled status.
 * Note: this register stores the *disabled* value, but for consistency with the
 * rest of the code, the function is named and used with standard true/false
 * values to indicate whether the sensor is enabled or disabled, respectively.
 *
 * @param enabled New temperature sensor enabled status
 * @see getTempSensorEnabled()
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_TEMP_DIS_BIT
 */
void MPU6050::setTempSensorEnabled(bool enabled) {
    // 1 is actually disabled here
    I2Cdev::writeBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_TEMP_DIS_BIT, !enabled);
}
/** Get clock source setting.
 * @return Current clock source setting
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_CLKSEL_BIT
 * @see MPU6050_PWR1_CLKSEL_LENGTH
 */
uint8_t MPU6050::getClockSource() {
    I2Cdev::readBits(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, buffer);
    return buffer[0];
}
/** Set clock source setting.
 * An internal 8MHz oscillator, gyroscope based clock, or external sources can
 * be selected as the MPU-60X0 clock source. When the internal 8 MHz oscillator
 * or an external source is chosen as the clock source, the MPU-60X0 can operate
 * in low power modes with the gyroscopes disabled.
 *
 * Upon power up, the MPU-60X0 clock source defaults to the internal oscillator.
 * However, it is highly recommended that the device be configured to use one of
 * the gyroscopes (or an external clock source) as the clock reference for
 * improved stability. The clock source can be selected according to the following table:
 *
 * <pre>
 * CLK_SEL | Clock Source
 * --------+--------------------------------------
 * 0       | Internal oscillator
 * 1       | PLL with X Gyro reference
 * 2       | PLL with Y Gyro reference
 * 3       | PLL with Z Gyro reference
 * 4       | PLL with external 32.768kHz reference
 * 5       | PLL with external 19.2MHz reference
 * 6       | Reserved
 * 7       | Stops the clock and keeps the timing generator in reset
 * </pre>
 *
 * @param source New clock source setting
 * @see getClockSource()
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_CLKSEL_BIT
 * @see MPU6050_PWR1_CLKSEL_LENGTH
 */
void MPU6050::setClockSource(uint8_t source) {
    I2Cdev::writeBits(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
}

// FIFO_COUNT* registers

/** Get current FIFO buffer size.
 * This value indicates the number of bytes stored in the FIFO buffer. This
 * number is in turn the number of bytes that can be read from the FIFO buffer
 * and it is directly proportional to the number of samples available given the
 * set of sensor data bound to be stored in the FIFO (register 35 and 36).
 * @return Current FIFO buffer size
 */
uint16_t MPU6050::getFIFOCount() {
    I2Cdev::readBytes(devAddr, MPU6050_RA_FIFO_COUNTH, 2, buffer);
    return (((uint16_t)buffer[0]) << 8) | buffer[1];
}

// FIFO_R_W register

/** Get byte from FIFO buffer.
 * This register is used to read and write data from the FIFO buffer. Data is
 * written to the FIFO in order of register number (from lowest to highest). If
 * all the FIFO enable flags (see below) are enabled and all External Sensor
 * Data registers (Registers 73 to 96) are associated with a Slave device, the
 * contents of registers 59 through 96 will be written in order at the Sample
 * Rate.
 *
 * The contents of the sensor data registers (Registers 59 to 96) are written
 * into the FIFO buffer when their corresponding FIFO enable flags are set to 1
 * in FIFO_EN (Register 35). An additional flag for the sensor data registers
 * associated with I2C Slave 3 can be found in I2C_MST_CTRL (Register 36).
 *
 * If the FIFO buffer has overflowed, the status bit FIFO_OFLOW_INT is
 * automatically set to 1. This bit is located in INT_STATUS (Register 58).
 * When the FIFO buffer has overflowed, the oldest data will be lost and new
 * data will be written to the FIFO.
 *
 * If the FIFO buffer is empty, reading this register will return the last byte
 * that was previously read from the FIFO until new data is available. The user
 * should check FIFO_COUNT to ensure that the FIFO buffer is not read when
 * empty.
 *
 * @return Byte from FIFO buffer
 */
uint8_t MPU6050::getFIFOByte() {
    I2Cdev::readByte(devAddr, MPU6050_RA_FIFO_R_W, buffer);
    return buffer[0];
}
void MPU6050::getFIFOBytes(uint8_t *data, uint8_t length) {
    if(length > 0){
        I2Cdev::readBytes(devAddr, MPU6050_RA_FIFO_R_W, length, data);
    } else {
    	*data = 0;
    }
}
/** Write byte to FIFO buffer.
 * @see getFIFOByte()
 * @see MPU6050_RA_FIFO_R_W
 */
void MPU6050::setFIFOByte(uint8_t data) {
    I2Cdev::writeByte(devAddr, MPU6050_RA_FIFO_R_W, data);
}

// WHO_AM_I register

/** Get Device ID.
 * This register is used to verify the identity of the device (0b110100, 0x34).
 * @return Device ID (6 bits only! should be 0x34)
 * @see MPU6050_RA_WHO_AM_I
 * @see MPU6050_WHO_AM_I_BIT
 * @see MPU6050_WHO_AM_I_LENGTH
 */
uint8_t MPU6050::getDeviceID() {
    I2Cdev::readBits(devAddr, MPU6050_RA_WHO_AM_I, MPU6050_WHO_AM_I_BIT, MPU6050_WHO_AM_I_LENGTH, buffer);
    return buffer[0];
}
/** Set Device ID.
 * Write a new ID into the WHO_AM_I register (no idea why this should ever be
 * necessary though).
 * @param id New device ID to set.
 * @see getDeviceID()
 * @see MPU6050_RA_WHO_AM_I
 * @see MPU6050_WHO_AM_I_BIT
 * @see MPU6050_WHO_AM_I_LENGTH
 */
void MPU6050::setDeviceID(uint8_t id) {
    I2Cdev::writeBits(devAddr, MPU6050_RA_WHO_AM_I, MPU6050_WHO_AM_I_BIT, MPU6050_WHO_AM_I_LENGTH, id);
}


