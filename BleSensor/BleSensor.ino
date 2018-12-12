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

void setup() {
    bleSensor = new BleSensor();
}

void loop() {
    if (heartRate == 50) {
        heartRate = 80;
    }
    bleSensor->setValue(heartRate);
    heartRate--;
    delay(500);
}
