/************************************************************************************
 * 	
 * 	Name    : MMA8452Q Library                         
 * 	Author  : Noah Shibley, NoMi Design Ltd. http://socialhardware.net       
 *			: Michael Grant, Krazatchu Design Systems. http://krazatchu.ca/
 * 	Date    : October 10th 2013                  
 * 	Version : 0.3                                              
 * 	Notes   : Arduino Library for use with the Freescale MMA8453Q and MMA8452Q via native WIRE with repeated start (was i2c of DSS circuits). 
              Some of the lib source from Kerry D. Wong
			  http://www.kerrywong.com/2012/01/09/interfacing-mma8453q-with-arduino/
 * 
 * 
 * 	This file is part of MMA8452Q.
 * 
 * 		    MMA8452Q is free software: you can redistribute it and/or modify
 * 		    it under the terms of the GNU General Public License as published by
 * 		    the Free Software Foundation, either version 3 of the License, or
 * 		    (at your option) any later version.
 * 
 * 		    AtTouch is distributed in the hope that it will be useful,
 * 		    but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 		    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 		    GNU General Public License for more details.
 * 
 * 		    You should have received a copy of the GNU General Public License
 * 		    along with MMA8452Q.  If not, see <http://www.gnu.org/licenses/>.
 * 
 ***********************************************************************************/


#ifndef MMA8452Q_H
#define MMA8452Q_H

#include "Arduino.h"

#include <Wire.h>

#define I2C_SDAPINaccel   21
#define I2C_SCLPINaccel   22

#define ODR_MMA8452 ODR_50

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


extern "C" void IRAM_ATTR accelISR(void) __attribute__ ((signal)); 


class MMA8452Q {

public:
  friend void accelISR(void);	
 
  MMA8452Q();

/***********************************************************
 * 
 * setI2CAddr
 *
 * set the i2c address of the MMA8453 to a new value, such as 0x1D
 *   
 ***********************************************************/
void setI2CAddr(int address);

/***********************************************************
 * 
 * dataMode
 *
 * set the device to return raw data values
 *   
 ***********************************************************/  
void dataMode(boolean highRes, int gScaleRange, int INTPin);

/***********************************************************
 * 
 * x
 *
 * returns the x axis value
 *   
 ***********************************************************/
int x(){ return x_; }

/***********************************************************
 * 
 * y
 *
 * returns the y axis value
 *   
 ***********************************************************/
int y() { return y_; }

/***********************************************************
 * 
 * z
 *
 * returns the z axis value
 *   
 ***********************************************************/
int z(){ return z_; }

/***********************************************************
 * 
 * ISRflag
 *
 * returns the ISRflag value
 *   
 ***********************************************************/
bool getISRflag(){ return ISRFlag; }

/***********************************************************
 * 
 * update
 *
 * update data values, or clear interrupts. Use at start of loop()
 *   
 ***********************************************************/
  void update();

private:
	
	void xyz(int& x,int& y, int& z);
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

