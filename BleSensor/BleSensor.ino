/**
 * @file BleSensor.ino
 * 
 * @brief test program for the BleSensor Class
 * 
 * @author Peter
 */

#include "BleSensor.hpp"

static BleSensor* bleSensor;
static uint8_t heartRate = 80;
static int16_t x = 32000;
static int16_t y = -32000;
static int16_t z = 0;

void setup() {
    bleSensor = new BleSensor();
}

void loop() {
    if (heartRate == 50) {
        heartRate = 80;
    }
    bleSensor->setHRValue(heartRate);
    heartRate--;
    
    bleSensor->setRespirationValue(x, y, z);
    x += 2000;
    y += 2000;
    z += 2000;
    delay(500);
}
