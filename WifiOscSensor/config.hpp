/**
 * @file config.h
 * @author Armin
 * @brief
 * @date 2019-01-25
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef CONFIG_H
#define CONFIG_H

/// 
const int ID = 2;

/// Pin configuration
#define MMA8452Q_intPin   32
#define I2C_SDAPINaccel   21
#define I2C_SCLPINaccel   22

#define BH1792_intPin     33
#define I2C_SDAPINhr      17
#define I2C_SCLPINhr      16

#define EDA_ADCpin        ADC1_GPIO34_CHANNEL

/// Configuration acceleration sensor MMA8452
#define ODR_MMA8452 ODR_50      // 50Hz
#define FULL_SCALE_RANGE FULL_SCALE_RANGE_4g
#define MMA8452Q_ADDRESS 0x1D

/// Configuration of heart rate sensor BH1792
#define GREEN_LED_CURRENT 5
#define SAMPLE_INTERVAL 20000   // 20ms --> 50Hz

/// Configuration of ADC for GSR sensor
#define ADC_WIDTH ADC_WIDTH_BIT_10
#define ADC_ATTEN ADC_ATTEN_DB_11

#endif