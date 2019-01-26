/**
 * @file GroveGSR.hpp
 * @author your name (you@domain.com)
 * @brief Class definitions for the GSR (galvanic skin response) sensor GroveGSR
 * 
 */

#ifndef GROVEGSR_H
#define GROVEGSR_H

#include <driver/adc.h>
#include "Arduino.h"
#include "config.hpp"

class GroveGSR
{
public:
/**
 * @brief Construct a new Grove GSR object
 * 
 */
    GroveGSR();

/**
 * @brief Setup ADC to be able to read GSR data from sensor
 * 
 * @param ADCpin ADC pin of ESP32
 */
    void initGsrADC(uint8_t ADCpin);

/**
 * @brief Read GSR value with ADC
 * 
 * @param ADCpin ADC pin of ESP32
 */
    void readGSR(uint8_t ADCpin);

/**
 * @brief Get the Gsr Average object
 * 
 * @return int 
 */
    int getGsrAverage(void);

private:
    int gsr_average;
    int sum;
};

#endif