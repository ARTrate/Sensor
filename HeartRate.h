/**
 *  @file       HeartRate.h
 *  @brief      Header file for HeartRate.cpp
 *  @date       2018.02.06
 *  @version    1.1.1
 */

#include "Arduino.h"
#include "BH1792.h"

#ifndef HEARTRATE_H
#define HEARTRATE_H

#define SPL_NUM (100)           /* Range of sampling number (0 - 99) */

#define COEF_BH (0x3FCCB062)    /* High pass filter b0, -b1 coefficient 0.1Hz/100Hz Q30 format */
/* Use (0x3F02946A) when 0.5Hz/100Hz Q30 format */
#define COEF_AH (0x3F9960C5)    /* High pass filter -a1     coefficient 0.1Hz/200Hz Q30 format */
/* Use (0x3E0528D5) when 0.5Hz/100Hz Q30 format */

#define MV_LENGTH (5)           /* Number of moving averaging for pulse detection */
#define TH_COEF (0.993)         /* Coefficient for pulse threshold (exponential decline) */

#define PEAK_MIN (63)           /* Ignore waveform as pulse under this value */

#define HR_LENGTH (5)           /* Number of average for pulse rate */
#define HR_1MIN_SPL (3200)      /* Number of sampling for 1 minute (60s*32Hz) */
#define HR_INT_MAX (300)        /* 300 bpm (60*100/20) */
#define HR_INT_MIN (20)         /* 20 bpm (60*100/300) */

/** Calculate pulse waveform and pulse rate
 */
class HeartRate
{

public:
    HeartRate();
    void initHRsensor(uint8_t intselect, uint8_t green_cur, uint8_t INTpin);
    void start_sample();
    bool readHRdata();
    void calcHR();

private:
    /* HR Sensor */
    BH1792 hrsensor;

    /* Pulse waveform */
    int32_t     _val;               /* Pulse waveform value */
    int32_t     _prev_val;          /* Previous value */
    int32_t     _reg_hpf;           /* High pass filter memory value */
    bool        _wave_flag;         /* Pulse waveform set flag */

    uint32_t    _sampling_num;      /* Sampling number */

    /* Moving averaging */
    int32_t     _mv_buf[MV_LENGTH]; /* Circular buffer */
    int32_t     _mv_idx;            /* Buffer index */

    /* Threshold for detecting pulse */
    int32_t     _detect_val;        /* Detection value */
    int32_t     _prev_dt_val;       /* Previous data */

    int32_t     _threshold_val;     /* Threshold value */
    int32_t     _prev_th_val;       /* Previous data */

    /* Pulse rate */
    int32_t     _hr_counter;        /* Counter for pulse rate */
    int32_t     _hr_buf[HR_LENGTH]; /* Circular buffer */
    int32_t     _hr_idx;            /* Buffer index */

    int32_t     _hr;                /* Pulse rate value */
    bool        _hr_flag;           /* Pulse rate set flag */
    bool        _sync_flag;         /* Pulse sync sign flag */

    /* Member functions */
    void interval_timer();
    int32_t hpf(int32_t val);
    bool detect_peak(int32_t val);
    void calc_hr();
};
#endif    /* HeartRate */