/**
 *  @file       HeartRate.cpp
 *  @brief      Measuring plethysmogram and calc heart rate
 *  @date       2018.02.06
 *  @version    1.1.1
 */
#include "HeartRate.h"

/** Constructor
 *  @param      bh_sda      Pin for SDA on BH1792GLC
 *  @param      bh_scl      Pin for SCL on BH1792GLC
 *  @param      bh_int      Pin for INT on BH1792GLC
 */
HeartRate::HeartRate()
{
    _val = 0;
    _wave_flag = false;
    _hr_flag = false;
    _sync_flag = false;
    _sampling_num = 0;
    _mv_idx = 0;
}

void HeartRate::initHRsensor(uint8_t intselect, uint8_t green_cur, uint8_t INTpin)
{
    hrsensor.initHRmeas(intselect, green_cur, INTpin);
}

void HeartRate::start_sample()
{
    hrsensor.startMeasurement();
}

bool HeartRate::readHRdata()
{
    return hrsensor.readHR();

    // if(_hr_flag) {
    //     _hr_flag = false;
    //     return true;
    // } else {
    //     return false;
    // }
}

void HeartRate::calcHR()
{
    _val = (int32_t)hrsensor.getGreenData();

    _val = -_val;
    //_val = hpf(_val);

    if(detect_peak(_val)) {     /* If detecting pulse */
        calc_hr();              /* Calculate pulse rate including flag set */
        Serial.print("HR: ");
        Serial.println(_hr,DEC);
    }
}

/** Interval timer
 */
void HeartRate::interval_timer()
{

    _val = -_val;

    _val = hpf(_val);                                   /* High pass filter (Comment out if not necessary) */
    _sampling_num = (_sampling_num + 1) % SPL_NUM;      /* Update sampling number */
    _wave_flag = true;                                  /* Set ready flag for pulse waveform */

    /* Pulse rate */
    if(detect_peak(_val)) {     /* If detecting pulse */
        calc_hr();              /* Calculate pulse rate including flag set */
    }
}

/** Fixed point high pass filter
 *  @param      val         Input value
 *  @return                 Output value
 *
 *  A/D value of mbed is Q6 format.
 *  Please shift in advance if necessary.
 */
int32_t HeartRate::hpf(int32_t val)
{
    int32_t reg, ret_val;
    int64_t temp_val;

    temp_val = (int64_t)COEF_AH * (int64_t)_reg_hpf;
    reg = val + (int32_t)(temp_val >> 30);
    ret_val = reg - _reg_hpf;
    temp_val = (int64_t)COEF_BH * (int64_t)ret_val;
    ret_val = (int32_t)(temp_val >> 30);
    _reg_hpf = reg;
    return ret_val;
}

/** Detect pulse peak
 *  @param      &val        Waveform data value
 *  @retval     true        Detected pulse peak
 *  @retval     false       No detection
 */
bool HeartRate::detect_peak(int32_t val)
{
    int i;
    bool retVal = false;

    /* Calculate differential of input value */
    _mv_buf[_mv_idx] = val - _prev_val;
    _prev_val = val;
    _mv_idx = (_mv_idx + 1) % MV_LENGTH;

    /* Calculate moving averaging */
    _detect_val = 0;
    for(i=0; i<MV_LENGTH; i++) {
        _detect_val += _mv_buf[i];
    }
    _detect_val = _detect_val / MV_LENGTH;

    /* Calculate exponential decline for threshold line */
    _threshold_val = (int32_t)((double)_prev_th_val * TH_COEF);

    if(_detect_val >= _threshold_val) {
        /* If exceeding threshold */
        if(_prev_dt_val < _prev_th_val) {
            /* If previous value is under threshold and over ignore value */
            if((_detect_val > PEAK_MIN) && (_hr_counter >= HR_INT_MIN)) {
                /* Detecting peak!!! */
                retVal = true;
            }
        }
        /* Previous threshold value is set to input value */
        _prev_th_val = _detect_val;
    } else {
        /* Previous threshold value is set to decline value */
        _prev_th_val = _threshold_val;
    }
    /* Update previous input value */
    _prev_dt_val = _detect_val;

    /* Increment pulse rate counter */
    _hr_counter++;

    return retVal;
}

/** Calculate pulse rate
 */
void HeartRate::calc_hr()
{
    int i;

    /* If pulse rate counter is within maximum value */
    if(_hr_counter <= HR_INT_MAX) {
        /* Calculate moving averaging */
        _hr_buf[_hr_idx] = _hr_counter;
        _hr_idx = (_hr_idx + 1) % HR_LENGTH;
        _hr = 0;
        for(i=0; i<HR_LENGTH; i++) {
            _hr += _hr_buf[i];
        }
        /* Set pulse rate value */
        _hr =  HR_1MIN_SPL * HR_LENGTH / _hr;
    } else {
        /* Pulse rate is set to invalid value */
        _hr = 0;
    }
    _hr_counter = 0;

    /* Set pulse rate flag */
    _hr_flag = true;
    _sync_flag = true;
}

