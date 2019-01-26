/**
 * @file BH1792.hpp
 * @author Armin
 * @brief Class definitions for the heart rate sensor BH1792
 * 
 */

#ifndef BH1792_H
#define BH1792_H


#include <Wire.h>
#include <bh1792glc_registers.hpp>
#include "Arduino.h"
#include "config.hpp"

extern "C" void hrISR(void) __attribute__ ((signal)); 

class BH1792
{
public:
    friend void hrISR(void);

/**
 * @brief Construct a new BH1792 object
 * 
 */
    BH1792();

/**
 * @brief Set the device to return green raw data values
 * 
 * @param intselect select itnerrupt type
 * @param green_cur set green LED current
 * @param INTpin select interrupt pin of ESP32
 */
    void initHRmeas(uint8_t intselect, uint8_t green_cur, uint8_t INTpin);

/**
 * @brief Reset all registers by setting SWRESET bit
 * 
 */
    void softReset(void);


/**
 * @brief Start measurement
 * 
 */
    void startMeasurement(void);

/**
 * @brief Stop measurement by setting SWRESET bit
 * 
 */
    void stopMeasurement(void);

/**
 * @brief Send measurement synchronization signal every 1 second
 * 
 */
    void sendSync(void);

/**
 * @brief Read green LED data and clear sensors interrupt flag
 * 
 * @return true data available
 * @return false 
 */
    bool readHR(void);

/**
 * @brief Get the Green Data object
 * 
 * @return uint16_t 
 */
    uint16_t getGreenData(void);

private:

/**
 * @brief Read up to 32 entries of sensors FIFO and clear WATERMARK interrupt
 * 
 * @param fifo_reg 
 */
    void readFIFOdata(uint16_t *fifo_reg);

/**
 * @brief Read green LED data from sensor
 * 
 */
    void readGreenData(void);

/**
 * @brief Clear sensors interrupt flag
 * 
 */
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