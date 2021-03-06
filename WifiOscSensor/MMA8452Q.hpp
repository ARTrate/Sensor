/**
 * @file MMA8452Q.hpp
 * @author Armin
 * @brief Class definitions for the acceleration sensor MMA8452Q
 * 
 * @copyright 
 * Some of the lib source from:
 * - Noah Shibley, NoMi Design Ltd. http://socialhardware.net
 * - Michael Grant, Krazatchu Design Systems. http://krazatchu.ca/
 * - Kerry D. Wong, http://www.kerrywong.com/2012/01/09/interfacing-mma8453q-with-arduino/
 * 
 */


#ifndef MMA8452Q_H
#define MMA8452Q_H

#include "Arduino.h"
#include "config.hpp"
#include <Wire.h>

#define INT_EN_DRDY 0x01
#define INT_DRDY_INT1PIN 0x01

#define REG_STATUS  0x00 //(R) Real time status
#define REG_OUT_X_MSB  0x01 //(R) [7:0] are 8 MSBs of 10-bit sample
#define REG_OUT_X_LSB  0x02 //(R) [7:6] are 2 LSBs of 10-bit sample
#define REG_OUT_Y_MSB  0x03 //(R) [7:0] are 8 MSBs of 10-bit sample
#define REG_OUT_Y_LSB  0x04 //(R) [7:6] are 2 LSBs of 10-bit sample
#define REG_OUT_Z_MSB  0x05 //(R) [7:0] are 8 MSBs of 10-bit sample
#define REG_OUT_Z_LSB  0x06 //(R) [7:6] are 2 LSBs of 10-bit sample
#define REG_SYSMOD  0x0b //(R) Current system mode
#define REG_INT_SOURCE  0x0c //(R) Interrupt status
#define REG_WHO_AM_I  0x0d //(R) Device ID (0x3A)
#define REG_XYZ_DATA_CFG  0xe //(R/W) Dynamic range settings
#define REG_HP_FILTER_CUTOFF  0x0f //(R/W) cut-off frequency is set to 16Hz @ 800Hz
#define REG_PL_STATUS  0x10 //(R) Landscape/Portrait orientation status
#define REG_PL_CFG  0x11 //(R/W) Landscape/Portrait configuration
#define REG_PL_COUNT  0x12 //(R) Landscape/Portrait debounce counter
#define REG_PL_BF_ZCOMP  0x13 //(R) Back-Front, Z-Lock trip threshold
#define REG_P_L_THS_REG  0x14 //(R/W) Portrait to Landscape trip angle is 29 degree
#define REG_FF_MT_CFG  0x15 //(R/W) Freefall/motion functional block configuration
#define REG_FF_MT_SRC  0x16 //(R) Freefall/motion event source register
#define REG_FF_MT_THS  0x17 //(R/W) Freefall/motion threshold register
#define REG_FF_MT_COUNT  0x18 //(R/W) Freefall/motion debounce counter
#define REG_TRANSIENT_CFG  0x1d //(R/W) Transient functional block configuration
#define REG_TRANSIENT_SRC  0x1e //(R) Transient event status register
#define REG_TRANSIENT_THS  0x1f //(R/W) Transient event threshold
#define REG_TRANSIENT_COUNT  0x20 //(R/W) Transient debounce counter
#define REG_PULSE_CFG  0x21 //(R/W) ELE, Double_XYZ or Single_XYZ
#define REG_PULSE_SRC  0x22 //(R) EA, Double_XYZ or Single_XYZ
#define REG_PULSE_THSX  0x23 //(R/W) X pulse threshold
#define REG_PULSE_THSY  0x24 //(R/W) Y pulse threshold
#define REG_PULSE_THSZ  0x25 //(R/W) Z pulse threshold
#define REG_PULSE_TMLT  0x26 //(R/W) Time limit for pulse
#define REG_PULSE_LTCY  0x27 //(R/W) Latency time for 2nd pulse
#define REG_PULSE_WIND  0x28 //(R/W) Window time for 2nd pulse
#define REG_ASLP_COUNT  0x29 //(R/W) Counter setting for auto-sleep
#define REG_CTRL_REG1  0x2a //(R/W) ODR  800 Hz, STANDBY mode
#define REG_CTRL_REG2  0x2b //(R/W) Sleep enable, OS Modes, RST, ST
#define REG_CTRL_REG3  0x2c //(R/W) Wake from sleep, IPOL, PP_OD
#define REG_CTRL_REG4  0x2d //(R/W) Interrupt enable register
#define REG_CTRL_REG5  0x2e //(R/W) Interrupt pin (INT1/INT2) map
#define REG_OFF_X  0x2f //(R/W) X-axis offset adjust
#define REG_OFF_Y  0x30 //(R/W) Y-axis offset adjust
#define REG_OFF_Z  0x31 //(R/W) Z-axis offset adjust

#define FULL_SCALE_RANGE_2g  0x00
#define FULL_SCALE_RANGE_4g  0x01
#define FULL_SCALE_RANGE_8g  0x02

// possible data rates
#define ODR_800 0
#define ODR_400 1
#define ODR_200 2
#define ODR_100 3
#define ODR_50 4
#define ODR_12 5
#define ODR_6 6
#define ODR_1 7

/**
 * @brief Set ISRflag when data ready
 * 
 */
extern "C" void IRAM_ATTR accelISR(void) __attribute__ ((signal)); 


class MMA8452Q {

public:
  friend void accelISR(void);	
 
 /**
  * @brief Construct a new MMA8452Q object
  * 
  */
  	MMA8452Q();

/**
 * @brief Set I2C address object
 * 
 * @param address 
 */
	void setI2CAddr(int address);

/**
 * @brief Initialize acceleration sensor (resolution, measurement range, interrupt pin, data rate)
 * 
 * @param highRes Choose highres mode (true) or lowres mode (false)
 * @param gScaleRange (2g, 4g or 8g)
 * @param INTPin select interrupt pin of ESP32
 */  
	void dataMode(boolean highRes, int gScaleRange, int INTPin);

/**
 * @brief return x axis value
 * 
 * @return int 
 */
	int x(){ return x_; }

/**
 * @brief return y axis value
 * 
 * @return int 
 */
	int y() { return y_; }

/**
 * @brief return z axis value
 * 
 * @return int 
 */
	int z(){ return z_; }

/**
 * @brief return ISRflag value
 * 
 * @return true 
 * @return false 
 */
	bool getISRflag(){ return ISRFlag; }

/**
 * @brief Update x, y and z axis values if ISRflag is set
 * 
 */
  	void update();

private:
/**
 * @brief Get accelerometer readings (x, y, z). This function also convers 2's complement number to signed integer result.
 * 
 * @param x 
 * @param y 
 * @param z 
 */	
	void xyz(int& x,int& y, int& z);

/**
 * @brief Clear sensors interrupt by reading acceleration data if ISRflag is set
 * 
 */
	void clearInterrupt();
	
	int x_,y_,z_;
	
	byte I2CAddr; 
	
	boolean highRes_;
	int gScaleRange_;
	boolean dataMode_;

	volatile bool ISRFlag;
	static MMA8452Q* pMMA8452Q; 	//ptr to MMA8452Q class for the ISR
 
};

#endif

