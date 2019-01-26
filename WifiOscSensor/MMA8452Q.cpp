/**
 * @file MMA8452Q.cpp
 * @author Armin
 * @brief Implementation
 * 
 * @copyright 
 * Some of the lib source from:
 * - Noah Shibley, NoMi Design Ltd. http://socialhardware.net
 * - Michael Grant, Krazatchu Design Systems. http://krazatchu.ca/
 * - Kerry D. Wong, http://www.kerrywong.com/2012/01/09/interfacing-mma8453q-with-arduino/
 * 
 */


#include "MMA8452Q.hpp"

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


void MMA8452Q::setI2CAddr(int address)
{
	I2CAddr = address;
}

void MMA8452Q::update()
{
	clearInterrupt();	
}


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

	ISRFlag = false; 
  }
}


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
	
    x = ((buf[0] << 4) | (buf[1] >> 4)) & 0xE;
    y = ((buf[2] << 4) | (buf[3] >> 4)) & 0xE;
    z = ((buf[4] << 4) | (buf[5] >> 4)) & 0xE;
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


void MMA8452Q::dataMode(bool highRes, int gScaleRange, int INTPin)
{
	highRes_ = highRes;
	gScaleRange_ = gScaleRange;
	dataMode_ = true;
	byte statusCheck = 0x00;
	byte activeMask = 0x01;
	byte resModeMask = 0x02;

	I2C_accel.begin(I2C_SDAPINaccel,I2C_SCLPINaccel,100000); //setup i2c

//setup interrupt
	pinMode(INTPin, INPUT);		// MMA8452 has push-pull stage at interrupt pin (for configuration see CtrlReg3)
	digitalWrite(INTPin, HIGH);
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


void IRAM_ATTR accelISR(void){
	MMA8452Q::pMMA8452Q->ISRFlag = true;
}
