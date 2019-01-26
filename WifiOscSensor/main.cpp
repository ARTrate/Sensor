/**
 * @file main.cpp
 * @author Armin
 * @brief Main with setup() and loop()
 * 
 */

#include "MMA8452Q.hpp"
#include "BH1792.hpp"
#include "OscSensor.hpp"
#include "WiFi.h"
#include "GroveGSR.hpp"
#include "config.hpp"

/// Wifi configuration
const char* ssid = "Artrate";
const char* pwd = "artratewifi";
const int port = 5005;
const char* serverIp = "192.168.43.39";

static OscSensor* oscSensor;
MMA8452Q accel;
BH1792 hr;
GroveGSR gsr;

hw_timer_t * tmr_BH1792sync = NULL;
volatile SemaphoreHandle_t timer0Semaphore;
void timer_isr(void);

void setup()
{
  Serial.begin(9600);

  WiFi.begin(ssid, pwd);
  while(WiFi.status() != WL_CONNECTED){
    delay(100);
    // without wifi nothing todo... 
  }
  oscSensor = new OscSensor(port, serverIp);

  accel.setI2CAddr(MMA8452Q_ADDRESS);
  accel.dataMode(true, FULL_SCALE_RANGE, MMA8452Q_intPin); //enable highRes 12bit, 4g range, ODR 50Hz

  gsr.initGsrADC(EDA_ADCpin);

  timer0Semaphore = xSemaphoreCreateBinary();
  tmr_BH1792sync = timerBegin(0,80,true);     // Timer 0 to use, TIMER_CLK = APD_CLK/80 = 1MHZ, count up
  timerAttachInterrupt(tmr_BH1792sync,timer_isr,true);
  timerAlarmWrite(tmr_BH1792sync, SAMPLE_INTERVAL, true);   // 50Hz timer (20ms)
  hr.initHRmeas(INT_SEL_ON_COMPLETE, GREEN_LED_CURRENT, BH1792_intPin);   // Interrupt select, green led current [0-63mA], interrupt pin
  timerAlarmEnable(tmr_BH1792sync);
}

void loop()
{
  // gsr.readGSR(EDA_ADCpin);
  // Serial.println(gsr.getGsrAverage());
  
  if(accel.getISRflag())
  {
    oscSensor->sendRr(accel.x(),accel.y(),accel.z());
  }  

  if (xSemaphoreTake(timer0Semaphore, 0) == pdTRUE){
    hr.startMeasurement();
  }

  if(hr.readHR()) {
    oscSensor->sendHr(hr.getGreenData());
  }
}

/**
 * @brief timer ISR
 * 
 */
void IRAM_ATTR timer_isr(void) {
  xSemaphoreGiveFromISR(timer0Semaphore, NULL);
}