/************************************************************************************
 * 	
 * 	Name    : BH1792 Library                         
 * 	Author  : 
 * 	Date    :                   
 * 	Version :                                               
 * 	Notes   : 
 * 
 * 
 * 	This file is part of BH1792.
 * 
 ***********************************************************************************/

#include <BH1792.h>

TwoWire I2C_hr = TwoWire(1);
BH1792* BH1792::pBH1792 = 0; 

BH1792::BH1792()
{
    pBH1792 = this;
    //default values
    i2caddress=0b1011011;
    sel_adc = SEL_ADC_GREEN;
    meas_mode = MSR_SINGLE_MEAS_MODE;
    led_en1 = (0x0<<6);
    led_en2 = (0x0<<7);
    led_cur1 = 0;
    led_cur2 = 0;
    ir_th = 0xFFFC;
    int_sel = INT_SEL_DISABLE;

    greenData_LEDoff = 0;
    greenData_LEDon = 0;

    ISRFlag_HR = false;
}

void BH1792::initHRmeas(uint8_t intselect, uint8_t green_cur, uint8_t INTpin)
{
    byte statusCheck = 0x00;
    
    //meas_mode = msr;
    led_cur1 = green_cur;
    int_sel = intselect;

    I2C_hr.begin(I2C_SDAPINhr,I2C_SCLPINhr,100000); //setup i2c

//setup interrupt
	pinMode(INTpin, INPUT_PULLUP);	//BH1792 hat Open-Drain-Ausgang am INT-Pin
	digitalWrite(INTpin, HIGH);		//?????
	attachInterrupt(INTpin,hrISR,FALLING);

    I2C_hr.beginTransmission(i2caddress); // transmit to device 
	I2C_hr.write(byte(PARTID_REG));       // sets register pointer to echo register 
	I2C_hr.endTransmission(false);        // stop transmitting (don't hang up - repeated start)
	I2C_hr.requestFrom (int(i2caddress), 1);
	while(I2C_hr.available())    // slave may send less than requested
	{ 
		statusCheck = I2C_hr.read(); // receive a byte as character
	}
	I2C_hr.endTransmission(true);      // stop transmitting - hang up -

    if(statusCheck != PART_ID) {
        Serial.println();
        Serial.println(statusCheck, HEX);
        while(1);
    }

    clearInterrupt();

// Settings for BH1792
    I2C_hr.beginTransmission(i2caddress); // transmit to device 
    I2C_hr.write(byte(MEAS_CONTROL1));      // sets register pointer
	I2C_hr.write(byte(RDY | sel_adc | meas_mode)); //Meas_Control1
    I2C_hr.write(byte(led_en1|led_cur1)); //Meas_Control2
    I2C_hr.write(byte(led_en2|led_cur2)); //Meas_Control3
    I2C_hr.write(byte(ir_th));            //Meas_Control4
    I2C_hr.write(byte(ir_th>>8));         //Meas_Control4
    I2C_hr.write(byte(int_sel));          //Meas_Control5
	I2C_hr.endTransmission(true);      // stop transmitting & hang up


    // I2C_hr.beginTransmission(i2caddress); // transmit to device 
	// I2C_hr.write(byte(MEAS_CONTROL1));       // sets register pointer to echo register 
	// I2C_hr.endTransmission(false);        // stop transmitting (don't hang up - repeated start)
	// I2C_hr.requestFrom (int(i2caddress), 1);
	// while(I2C_hr.available())    // slave may send less than requested
	// { 
	// 	statusCheck = I2C_hr.read(); // receive a byte as character
	// }
	// I2C_hr.endTransmission(true);      // stop transmitting - hang up -

    // Serial.print("Meas_Ctrl1: ");
    // Serial.println(statusCheck, BIN);
}

void BH1792::softReset(void)
{
    I2C_hr.beginTransmission(i2caddress); // transmit to device
    I2C_hr.write(byte(RESET));      // sets register pointer
    I2C_hr.write(byte(SWRESET));          // Reset all registers when writing '1'
    I2C_hr.endTransmission(true);         // stop transmitting & hang up
}

void BH1792::clearInterrupt(void)
{
    byte statusCheck = 0x00;

    I2C_hr.beginTransmission(i2caddress); // transmit to device 
	I2C_hr.write(byte(INT_CLEAR));       // sets register pointer to echo register 
	I2C_hr.endTransmission(false);        // stop transmitting (don't hang up - repeated start)
	I2C_hr.requestFrom (int(i2caddress), 1);
	while(I2C_hr.available())    // slave may send less than requested
	{ 
		statusCheck = I2C_hr.read(); // receive a byte as character
	}
	I2C_hr.endTransmission(true);      // stop transmitting - hang up -
}

void BH1792::startMeasurement(void)
{
    I2C_hr.beginTransmission(i2caddress); // transmit to device
    I2C_hr.write(byte(MEAS_START));      // sets register pointer
    I2C_hr.write(byte(MEAS_ST));          // Measure start flag
    I2C_hr.endTransmission(true);         // stop transmitting & hang up
}

void BH1792::stopMeasurement(void)
{
    softReset();
}

void BH1792::sendSync(void)
{
    I2C_hr.beginTransmission(i2caddress); // transmit to device
    I2C_hr.write(byte(MEAS_SYNC));        // sets register pointer
    I2C_hr.write(byte(MEAS_SYNC_BITS));   // Measure sync every 1 second
    I2C_hr.endTransmission(true);         // stop transmitting & hang up
}

bool BH1792::readHR(void)
{
    if(ISRFlag_HR)
    {
        readGreenData();
        clearInterrupt();

        // Serial.print("Off: ");
        // Serial.print(greenData_LEDoff);
        // Serial.print(" On: ");
        // Serial.println(greenData_LEDon);

        ISRFlag_HR=false;
        return true;
    }
    return false;
}

uint16_t BH1792::getGreenData(void)
{
    return greenData_LEDon;
}

void BH1792::readFIFOdata(uint16_t *fifo_reg)
{
    if(ISRFlag_HR) {
        Serial.println("hrISR");

        ISRFlag_HR=false;
    }

}

void BH1792::readGreenData(/*uint16_t *green_reg*/ void)
{
    uint8_t count = 4;
    uint8_t reg_val[count] = {0};

    I2C_hr.beginTransmission(i2caddress); // transmit to device 
	I2C_hr.write(byte(GREENDATA_LEDOFF_L));      // sets register pointer to echo register 
 	I2C_hr.endTransmission(false);      // stop transmitting (don't hang up - repeated start)
	I2C_hr.requestFrom (int(i2caddress), count); 
    if(count >= I2C_hr.available())    // if count bytes were received
    { 
	    for (int i = 0;  i <= (count-1); i++)
	    {
	        reg_val[i] = I2C_hr.read(); 
	    }
    }
    I2C_hr.endTransmission(true); // stop transmitting - hang up -

    // Serial.print("reg0: ");
    // Serial.print(reg_val[0]);
    // Serial.print(", reg1: ");
    // Serial.print(reg_val[1]);
    // Serial.print(", reg2: ");
    // Serial.print(reg_val[2]);
    // Serial.print(", reg3: ");
    // Serial.println(reg_val[3]);

    greenData_LEDoff = (reg_val[1] << 8) | reg_val[0];
    greenData_LEDon = (reg_val[3] << 8) | reg_val[2];
}

/***********************************************************
 * 
 * hrISR
 *
 * 
 *   
 ***********************************************************/
void hrISR(void){
	BH1792::pBH1792->ISRFlag_HR = true;
}

