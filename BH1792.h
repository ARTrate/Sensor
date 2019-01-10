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

#ifndef BH1792_H
#define BH1792_H


#include <Wire.h>
#include <bh1792glc_registers.h>
#include "Arduino.h"

#define I2C_SDAPINhr      17
#define I2C_SCLPINhr      16

extern "C" void hrISR(void) __attribute__ ((signal)); 

class BH1792
{
public:
    friend void hrISR(void);

    BH1792();
    //~BH1792();

/***********************************************************
 * 
 * initHRmeas
 *
 * Set the device to return green raw data values
 *   
 ***********************************************************/
    void initHRmeas(uint8_t intselect, uint8_t green_cur, uint8_t INTpin);

/***********************************************************
 * 
 * softReset
 *
 * Reset all registers by setting SWRESET bit
 *   
 ***********************************************************/
    void softReset(void);


/***********************************************************
 * 
 * startMeasurement
 *
 * Start measurement/enable sensor
 *   
 ***********************************************************/
    void startMeasurement(void);

/***********************************************************
 * 
 * stopMeasurement
 *
 * Stop measurement by setting SWRESET bit
 *   
 ***********************************************************/
    void stopMeasurement(void);

/***********************************************************
 * 
 * sendSync
 *
 * Send measurement synchronization signal every 1 second
 *   
 ***********************************************************/
    void sendSync(void);

/***********************************************************
 * 
 * readHR
 *
 * 
 *   
 ***********************************************************/
    bool readHR(void);

/***********************************************************
 * 
 * readHR
 *
 * 
 *   
 ***********************************************************/
    uint16_t getGreenData(void);

private:

/***********************************************************
 * 
 * readFIFOdata
 *
 * Read up to 32 entries of FIFO and clear WATERMARK interrupt
 *   
 ***********************************************************/
    void readFIFOdata(uint16_t *fifo_reg);

/***********************************************************
 * 
 * readGreenData
 *
 * Read up to 32 entries of FIFO and clear WATERMARK interrupt
 *   
 ***********************************************************/
    void readGreenData(/*uint16_t *green_reg*/void);

/***********************************************************
 * 
 * clearInterrupt
 *
 * Clear WATERMARK interrupt flag in BH1792
 *   
 ***********************************************************/
    void clearInterrupt(void);


    uint8_t i2caddress;
    uint8_t sel_adc;
    uint8_t meas_mode;
    uint8_t led_en1;
    uint8_t led_en2;
    uint8_t led_cur1;
    uint8_t led_cur2;
    uint16_t ir_th;
    uint8_t int_sel;

    uint16_t greenData_LEDoff;
    uint16_t greenData_LEDon;

    volatile boolean ISRFlag_HR;
    static BH1792 * pBH1792; //ptr to BH1792 class for the ISR
};

#endif