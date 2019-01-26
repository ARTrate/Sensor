/*
The MIT License (MIT)
Copyright (c) 2017 Rohm Semiconductor
Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:
The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __BH1792GLC_REGISTERS_H__
#define __BH1792GLC_REGISTERS_H__
/************************************* registers ******************************/
#define MANUFACTURER_REG 0x0F
// WHO_AM_I -value
#define PARTID_REG 0x10
// Soft reset
#define RESET 0x40
// System control setting
#define MEAS_CONTROL1 0x41
// LED 1 & 2 control register
#define MEAS_CONTROL2 0x42
// LED 3 control register
#define MEAS_CONTROL3 0x43
// IR Interrupt Threshold Value [7:0]
#define MEAS_CONTROL4_L 0x44
// IR Interrupt Threshold Value [15:8]
#define MEAS_CONTROL4_H 0x45
#define MEAS_CONTROL5 0x46
#define MEAS_START 0x47
#define MEAS_SYNC 0x48
#define FIFO_LEV 0x4B
// FIFO Output data 0 [7:0]
#define FIFO_DATA0_L 0x4C
// FIFO Output data 0 [15:8]
#define FIFO_DATA0_H 0x4D
// FIFO Output data 1 [7:0]
#define FIFO_DATA1_L 0x4E
// FIFO Output data 1 [15:8]
#define FIFO_DATA1_H 0x4F
// IR Data Count Value during no LED emission [7:0]
#define IRDATA_LEDOFF_L 0x50
// IR Data Count Value during no LED emission [15:8]
#define IRDATA_LEDOFF_H 0x51
// IR Data Count Value during LED emission [7:0]
#define IRDATA_LEDON_L 0x52
// IR Data Count Value during LED emission [15:8]
#define IRDATA_LEDON_H 0x53
// Green Data Count Value during no LED emission [7:0]
#define GREENDATA_LEDOFF_L 0x54
// Green Data Count Value during no LED emission [15:8]
#define GREENDATA_LEDOFF_H 0x55
// Green Data Count Value during LED emission [7:0]
#define GREENDATA_LEDON_L 0x56
// Green Data Count Value during LED emission [15:8]
#define GREENDATA_LEDON_H 0x57
// IR threshold judgement and measurement completion interrupt is cleared when reading this register.
#define INT_CLEAR 0x58


/***************************************** registers bits *****************************/
#define MANUFACTURER_ID (0xE0 << 0)
#define PART_ID (0x0E << 0)
// 1 : Software reset is performed
#define SWRESET (0x01 << 7)
// 1 : OSC block is active, 0: prohibited
#define RDY (0x01 << 7)
// Green mode, leds 1 & 2 active
#define SEL_ADC_GREEN (0x00 << 4)
// IR mode, led 3 active. Can be used only in single and non synch modes.
#define SEL_ADC_IR (0x01 << 4)
// 32 Hz synchronous mode
#define MSR_32HZ (0x00 << 0)
// 128 Hz synchronous mode
#define MSR_128HZ (0x01 << 0)
// 64 Hz synchronous mode
#define MSR_64HZ (0x02 << 0)
// 256 Hz synchronous mode
#define MSR_256HZ (0x03 << 0)
#define MSR_PROHIBITED (0x04 << 0)
// 1024 Hz synchronous mode
#define MSR_1024HZ (0x05 << 0)
// non synchronized measurement mode
#define MSR_NON_SYNCH_MODE (0x06 << 0)
// single measurement mode
#define MSR_SINGLE_MEAS_MODE (0x07 << 0)
// LED driver mode, for usage see datasheet
#define LED_EN2 (0x01 << 7)
// No interrupt output.
#define INT_SEL_DISABLE (0x00 << 0)
// Watermark interrupt FIFO, fires when number of stored samples reaches 32. Cleared when number of samples falls below 32.
#define INT_SEL_FIFO_WATERMARK (0x01 << 0)
// IR threshold judgement interrupt. Used in non synchroniozed mode.
#define INT_SEL_IR_THRESHOLD (0x02 << 0)
// Measurement completion interrupt. In single mode.
#define INT_SEL_ON_COMPLETE (0x03 << 0)
// Flag of start measurement. MEAS_ST=1 after RDY=1 starts measurement. In single measurement mode MEAS_ST=1 restarts measurement.
#define MEAS_ST (0x01 << 0)
// Used in synhcronized measurement mode. Write once per second.
#define MEAS_SYNC_BITS (0x01 << 0)
 
 
 /**************registers bit masks */
#define MANUFACTURER_MASK 0xFF
#define PART_MASK 0xFF
// Select LED omitting frequency
#define SEL_ADC_MASK 0x10
// Measurement mode
#define MSR_MASK 0x07
#define LED_EN1_MASK 0xC0
#define LED_CURRENT1_MASK 0x3F
#define LED_CURRENT2_MASK 0x3F
// Interrupt function select
#define INT_SEL_MASK 0x03
#define FIFO_LEV_LEVEL_MASK 0x3F
#endif