/************************************************************************************
 * 	
 * 	Name    : MMA8453_n0m1 Library Example: DataMode                       
 * 	Author  : Noah Shibley, NoMi Design Ltd. http://n0m1.com                       
 *		    : Michael Grant, Krazatchu Design Systems. http://krazatchu.ca/
 * 	Date    : May 5th 2013                                    
 * 	Version : 0.2                                              
 * 	Notes   : Arduino Library for use with the Freescale MMA8453Q via Arduino native WIRE with repeated start (was i2c of DSS circuits). 
 *
 ***********************************************************************************/


#include "MMA8452Q.h"
// #include <BH1792.h>
#include "HeartRate.h"
#include "BleSensor.h"

#define MMA8452Q_intPin   32
#define BH1792_intPin     33

MMA8452Q accel;
//BH1792 hr;
HeartRate hr;

static BleSensor* bleSensor;
static int16_t xAccel[3] = {0}; 
static int16_t yAccel[3] = {0}; 
static int16_t zAccel[3] = {0};
uint8_t accel_rxcount = 0;

hw_timer_t * tmr_BH1792sync = NULL;
volatile SemaphoreHandle_t timer0Semaphore;

void timer_isr(void);

void setup()
{
  bleSensor = new BleSensor();

  // timer0Semaphore = xSemaphoreCreateBinary();
  // tmr_BH1792sync = timerBegin(0,80,true);     // Timer 0 to use, TIMER_CLK = APD_CLK/80 = 1MHZ, count up
  // timerAttachInterrupt(tmr_BH1792sync,timer_isr,true);
  // timerAlarmWrite(tmr_BH1792sync, 31250, true);   // 32Hz timer (31.25ms)

  Serial.begin(9600);

  accel.setI2CAddr(0x1D); //change your device address if necessary, default is 0x1D
  accel.dataMode(true, FULL_SCALE_RANGE_4g, MMA8452Q_intPin); //enable highRes 12bit, 4g range [2g,4g,8g], ODR 50Hz

  // hr.initHRsensor(INT_SEL_ON_COMPLETE, 1, BH1792_intPin);   // Interrupt select, green led current [0-63mA], interrupt pin
  // timerAlarmEnable(tmr_BH1792sync);
}

void loop()
{
  if(accel.getISRflag())
  {
    accel.update();
    xAccel[accel_rxcount] = accel.x();
    yAccel[accel_rxcount] = accel.y();
    zAccel[accel_rxcount] = accel.z();

    accel_rxcount++;
    if(accel_rxcount>=3) {
      bleSensor->setRespirationValue(xAccel, yAccel, zAccel);
      accel_rxcount=0;
    }
  }
  
  

  // if (xSemaphoreTake(timer0Semaphore, 0) == pdTRUE){
  //   hr.start_sample();
  // }

  // if(hr.readHRdata()) {
  //   hr.calcHR();
  // }

  
  // Serial.print("x: ");
  // Serial.print(accel.x());
  // Serial.print(" y: ");
  // Serial.print(accel.y());
  // Serial.print(" z: ");
  // Serial.println(accel.z());

  //delay(1000); // Delay here for visibility
}


void IRAM_ATTR timer_isr(void) {
  xSemaphoreGiveFromISR(timer0Semaphore, NULL);
}