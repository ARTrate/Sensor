/**
 * @file GroveGSR.cpp
 * @author Armin
 * @brief Implementation
 * 
 */
#include "GroveGSR.hpp"


GroveGSR::GroveGSR()
{
    gsr_average=0;
    sum=0;
}

void GroveGSR::initGsrADC(uint8_t ADCpin)
{
    adc1_config_width(ADC_WIDTH);
    adc1_config_channel_atten((adc1_channel_t)ADCpin, ADC_ATTEN); // Gain 11dB = 3,6V Max
}


void GroveGSR::readGSR(uint8_t ADCpin)
{
    sum=0;
    for(int i=0;i<10;i++)      //Average 10 measurements to remove the glitch
    {
        sum += adc1_get_raw((adc1_channel_t)ADCpin);
        delay(10);
    }
    gsr_average = sum/10;
    Serial.println(gsr_average);
}


int GroveGSR::getGsrAverage(void) {
    return gsr_average;
}

