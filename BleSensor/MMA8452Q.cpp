/************************************************************************************
 * 	
 * 	Name    : MMA8452Q Library                         
 * 	Author  : Noah Shibley, NoMi Design Ltd. http://socialhardware.net      
 *			: Michael Grant, Krazatchu Design Systems. http://krazatchu.ca/
 * 	Date    : October 10th 2013                                    
 * 	Version : 0.3                                              
 * 	Notes   : Arduino Library for use with the MMA8452Q via native I2C_accel with repeated start. 
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

#include "MMA8452Q.h"

TwoWire I2C_accel = TwoWire(0);

MMA8452Q* MMA8452Q::pMMA8452Q = 0; 

MMA8452Q::MMA8452Q()
{
	pMMA8452Q = this;
	dataMode_ = false;
	ISRFlag = false;
	I2CAddr = 0x1c; //The i2C address of the MMA8453 chip. 0x1D is another common value.
	gScaleRange_ = 2;  //default 2g

}

/***********************************************************
 * 
 * setI2CAddr
 *
 * 
 *   
 ***********************************************************/
void MMA8452Q::setI2CAddr(int address)
{
	I2CAddr = address;
}

/***********************************************************
 * 
 * update
 *
 * 
 *   
 ***********************************************************/
void MMA8452Q::update()
{
	clearInterrupt();	
}

/***********************************************************
 * 
 * clearInterrupt
 *
 * 
 *   
 ***********************************************************/
void MMA8452Q::clearInterrupt()
{
	
  if(ISRFlag)
  {	
	byte sourceSystem=0x00;
	
	I2C_accel.beginTransmission(I2CAddr); // transmit to device 
	I2C_accel.write(byte(REG_INT_SOURCE));      // sets register pointer to echo register 
	I2C_accel.endTransmission(false);      // stop transmitting (don't hang up - repeated start)
	I2C_accel.requestFrom (int(I2CAddr), 1);
	while(I2C_accel.available())    // slave may send less than requested
	{ 
		sourceSystem = I2C_accel.read(); // receive a byte as character
	}
	I2C_accel.endTransmission(true);      // stop transmitting - hang up - 

	if((sourceSystem&0x01) == 0x01)  //Data Ready
	{
		xyz(x_,y_,z_);
	}	
	else {
		Serial.println("Error");
	}

	// Serial.print("x: ");
	// Serial.print(x_);
  	// Serial.print(" y: ");
  	// Serial.print(y_);
  	// Serial.print(" z: ");
  	// Serial.println(z_);

	ISRFlag = false; 
  }
}



/*************************************************************
* 
* xyz
* 
* Get accelerometer readings (x, y, z)
* 
* This function also convers 2's complement number to
* signed integer result.
*
*************************************************************/
void MMA8452Q::xyz(int& x, int& y, int& z)
{

  byte buf[6] = {0};

  if (highRes_) 
  {
    I2C_accel.beginTransmission(I2CAddr); // transmit to device 
	I2C_accel.write(byte(REG_OUT_X_MSB));      // sets register pointer to echo register 
    I2C_accel.endTransmission(false);      // stop transmitting but don't hang up - repeated start
    I2C_accel.requestFrom (int(I2CAddr), 6);
    if(6 <= I2C_accel.available())    // if 6 bytes were received
    { 
	   buf[0] = I2C_accel.read(); 
	   buf[1] = I2C_accel.read(); 
	   buf[2] = I2C_accel.read(); 
	   buf[3] = I2C_accel.read(); 
	   buf[4] = I2C_accel.read(); 
	   buf[5] = I2C_accel.read(); 
    }
	I2C_accel.endTransmission(true);      // stop transmitting - hang up - 
	
    x = (buf[0] << 4) | (buf[1] >> 4) & 0xE;
    y = (buf[2] << 4) | (buf[3] >> 4) & 0xE;
    z = (buf[4] << 4) | (buf[5] >> 4) & 0xE;
  }
  else 
  {
    I2C_accel.beginTransmission(I2CAddr); // transmit to device 
	I2C_accel.write(byte(REG_OUT_X_MSB));      // sets register pointer to echo register 
    I2C_accel.endTransmission(false);      // stop transmitting but don't hang up - repeated start
    I2C_accel.requestFrom (int(I2CAddr), 3);
    if(3 <= I2C_accel.available())    // if 3 bytes were received
    { 
	   buf[0] = I2C_accel.read(); 
	   buf[1] = I2C_accel.read(); 
	   buf[2] = I2C_accel.read(); 
    }
	I2C_accel.endTransmission(true);      // stop transmitting - hang up - 
   
    x = buf[0] << 4;
    y = buf[1] << 4;
    z = buf[2] << 4;
  }
  
  	if (x > 2047) x = x - 4096;
    if (y > 2047) y = y - 4096;
    if (z > 2047) z = z - 4096;
}

/***********************************************************
 * 
 * dataMode
 *
 * 
 *   
 ***********************************************************/
void MMA8452Q::dataMode(boolean highRes, int gScaleRange, int INTPin)
{
	highRes_ = highRes;
	gScaleRange_ = gScaleRange;
	dataMode_ = true;
	byte statusCheck = 0x00;
	byte activeMask = 0x01;
	byte resModeMask = 0x02;

	I2C_accel.begin(I2C_SDAPINaccel,I2C_SCLPINaccel,100000); //setup i2c

//setup interrupt
	pinMode(INTPin, INPUT);		//MMA8452 hat Push-Pull-Stufe an INT-Pins (Einstellung siehe CtrlReg3)
	digitalWrite(INTPin, HIGH);		//?????
	attachInterrupt(INTPin,accelISR,FALLING);
	
//register settings must be made in standby mode
	I2C_accel.beginTransmission(I2CAddr); // transmit to device 
	I2C_accel.write(byte(REG_CTRL_REG1));      // sets register pointer to echo register 
	I2C_accel.endTransmission(false);      // stop transmitting (don't hang up - repeated start)
	I2C_accel.requestFrom (int(I2CAddr), 1);
	while(I2C_accel.available())    // slave may send less than requested
	{ 
		statusCheck = I2C_accel.read(); // receive a byte as character
	}
	I2C_accel.endTransmission(true);      // stop transmitting - hang up - 

	I2C_accel.beginTransmission(I2CAddr); // transmit to device 
    I2C_accel.write(byte(REG_CTRL_REG1));      // sets register pointer
	I2C_accel.write(byte((statusCheck & ~activeMask)));   
	I2C_accel.endTransmission(true);      // stop transmitting & hang up

// set measurement range
	I2C_accel.beginTransmission(I2CAddr); // transmit to device 
    I2C_accel.write(byte(REG_XYZ_DATA_CFG));      // sets register pointer
	I2C_accel.write(byte(gScaleRange_));   
	I2C_accel.endTransmission(true);      // stop transmitting & hang up
	
    
//set highres 12bit or lowres 8bit
    I2C_accel.beginTransmission(I2CAddr); // transmit to device 
	I2C_accel.write(byte(REG_CTRL_REG1));      // sets register pointer to echo register 
	I2C_accel.endTransmission(false);      // stop transmitting (don't hang up - repeated start)
	I2C_accel.requestFrom (int(I2CAddr), 1);
	while(I2C_accel.available())    // slave may send less than requested
	{ 
		statusCheck = I2C_accel.read(); // receive a byte as character
	}
	I2C_accel.endTransmission(true);      // stop transmitting - hang up -
	
	if(highRes){
	    I2C_accel.beginTransmission(I2CAddr); // transmit to device 
		I2C_accel.write(byte(REG_CTRL_REG1));      // sets register pointer
		I2C_accel.write(byte(statusCheck & ~resModeMask));   
		I2C_accel.endTransmission(true);      // stop transmitting & hang up
	}
    else { 
  		I2C_accel.beginTransmission(I2CAddr); // transmit to device 
		I2C_accel.write(byte(REG_CTRL_REG1));      // sets register pointer
		I2C_accel.write(byte(statusCheck | resModeMask));   
		I2C_accel.endTransmission(true);      // stop transmitting & hang up
	}

// Output Data Rate 50Hz
	I2C_accel.beginTransmission(I2CAddr); // transmit to device 
	I2C_accel.write(byte(REG_CTRL_REG1));      // sets register pointer 
	I2C_accel.endTransmission(false);      // stop transmitting (don't hang up - repeated start)
	I2C_accel.requestFrom (int(I2CAddr), 1);
	while(I2C_accel.available())    // slave may send less than requested
	{ 
		statusCheck = I2C_accel.read(); // receive a byte as character
	}
	I2C_accel.endTransmission(true);      // stop transmitting - hang up - 	
	
    I2C_accel.beginTransmission(I2CAddr); // transmit to device 
	I2C_accel.write(byte(REG_CTRL_REG1));      // sets register pointer
	I2C_accel.write(byte(statusCheck | (ODR_MMA8452<<3)));   
	I2C_accel.endTransmission(true);      // stop transmitting & hang up

// enable data-ready interrupt
	I2C_accel.beginTransmission(I2CAddr); // transmit to device 
	I2C_accel.write(byte(REG_CTRL_REG4));      // sets register pointer
	I2C_accel.write(byte(INT_EN_DRDY));   
	I2C_accel.endTransmission(true);      // stop transmitting & hang up

// route data-ready interrupt to INT1-Pin
	I2C_accel.beginTransmission(I2CAddr); // transmit to device 
	I2C_accel.write(byte(REG_CTRL_REG5));      // sets register pointer
	I2C_accel.write(byte(INT_DRDY_INT1PIN));   
	I2C_accel.endTransmission(true);      // stop transmitting & hang up

//active Mode
 	I2C_accel.beginTransmission(I2CAddr); // transmit to device 
	I2C_accel.write(byte(REG_CTRL_REG1));      // sets register pointer to echo register 
	I2C_accel.endTransmission(false);      // stop transmitting (don't hang up - repeated start)
	I2C_accel.requestFrom (int(I2CAddr), 1);
	while(I2C_accel.available())    // slave may send less than requested
	{ 
		statusCheck = I2C_accel.read(); // receive a byte as character
	}
	I2C_accel.endTransmission(true);      // stop transmitting - hang up - 	
	
    I2C_accel.beginTransmission(I2CAddr); // transmit to device 
	I2C_accel.write(byte(REG_CTRL_REG1));      // sets register pointer
	I2C_accel.write(byte(statusCheck | activeMask));   
	I2C_accel.endTransmission(true);      // stop transmitting & hang up		
}

/***********************************************************
 * 
 * accelISR
 *
 * 
 *   
 ***********************************************************/
void IRAM_ATTR accelISR(void){
	MMA8452Q::pMMA8452Q->ISRFlag = true;
}
