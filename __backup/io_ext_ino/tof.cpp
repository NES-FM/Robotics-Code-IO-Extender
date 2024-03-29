#include "tof.h"

// General
tof::tof(int sensor_type, int offset_x, int offset_y, int offset_a, int xshut) 
{ 
    _sensor_type = sensor_type;
    _xshut = xshut;
    _offset_x = offset_x;
    _offset_y = offset_y;
    _offset_a = offset_a; 

    if (_sensor_type == TOF_SENSOR_VL53L0X)
        vl53l0x_sensor = new VL53L0X();
    else if (_sensor_type == TOF_SENSOR_VL53l1X)
        vl53l1x_sensor = new VL53L1X();
    else if (_sensor_type == TOF_SENSOR_VL6180X)
        vl6180x_sensor = new Adafruit_VL6180X();
}

void tof::init_xshut()
{
    if (_xshut != -1) 
    {
        pinMode(_xshut, OUTPUT);
        digitalWrite(_xshut, LOW);
    }
}

void tof::enable(bool enabled) {
    _enabled = enabled; 
    if (!enabled)
        _error = TOF_ERROR_NOT_ENABLED; 
}

void tof::init(uint8_t address)
{
    if (_enabled)
    {
        if (_sensor_type == TOF_SENSOR_VL53L0X)
            _vl53l0x_begin(address);
        else if (_sensor_type == TOF_SENSOR_VL53l1X)
            _vl53l1x_begin(address);
        else if (_sensor_type == TOF_SENSOR_VL6180X)
            _vl6180x_begin(address);
    }
    else
    {
        logln("Not Enabled!");
    }
}

void tof::setLongRangeMode(bool mode)
{
    if (_enabled && _sensor_type == TOF_SENSOR_VL53L0X)
    {
        _vl53l0x_setLongRangeMode(mode);
    }
}

void tof::setHighSpeed(bool mode)
{
    if (_enabled && _sensor_type == TOF_SENSOR_VL53L0X)
    {
        _vl53l0x_setHighSpeed(mode);
    }
}

void tof::setHighAccuracy(bool mode)
{
    if (_enabled && _sensor_type == TOF_SENSOR_VL53L0X)
    {
        _vl53l0x_setHighAccuracy(mode);
    }
}

void tof::holdReset()
{
    if (_xshut != -1)
    {
        digitalWrite(_xshut, LOW);
    }
}

void tof::releaseReset()
{
    if (_xshut != -1)
    {
        digitalWrite(_xshut, HIGH);
    }
}

void tof::reset()
{
    if (_xshut != -1)
    {
        this->holdReset();
        delay(10);
        this->releaseReset();
    }
}

String tof::getMeasurementErrorString()
{
    switch (_error)
    {
        case TOF_ERROR_NONE:
            return F("None");
        case TOF_ERROR_NOT_ENABLED:
            return F("Not enabled");
        case TOF_ERROR_TIMEOUT:
            return F("Timeout");
        case TOF_ERROR_MAX_DISTANCE:
            return F("Max distance exceeded");
        case TOF_ERROR_FAILED_TO_INITIALIZE:
            return F("Failed to initialize");
        case TOF_ERROR_MIN_DISTANCE:
            return F("Min distance exceeded");
        case TOF_ERROR_SYSERROR:
            return F("System error");
        case TOF_ERROR_ECE_ERROR:
            return F("ECE failure");
        case TOF_ERROR_NOCONVERGENCE:
            return F("No convergence");
        case TOF_ERROR_SNR_ERROR:
            return F("Signal/Noise Ratio Error");
    }
    return F("Unknown error");
}

void tof::changeAddress(uint8_t address)
{
    if (_enabled)
    {
        if (_sensor_type == TOF_SENSOR_VL53L0X)
            _vl53l0x_changeAddress(address);
        else if (_sensor_type == TOF_SENSOR_VL53l1X)
            _vl53l1x_changeAddress(address);
        else if (_sensor_type == TOF_SENSOR_VL6180X)
            _vl6180x_changeAddress(address);
    }
}

void tof::setContinuous(bool mode, uint32_t period_ms)
{
    if (_enabled)
    {
        if (_sensor_type == TOF_SENSOR_VL53L0X)
            _vl53l0x_setContinuous(mode, period_ms);
        else if (_sensor_type == TOF_SENSOR_VL53l1X)
            _vl53l1x_setContinuous(mode, period_ms);
        else if (_sensor_type == TOF_SENSOR_VL6180X)
            _vl6180x_setContinuous(mode, period_ms);
    }
}

uint16_t tof::getMeasurement()
{
    if (_enabled)
    {
        if (_sensor_type == TOF_SENSOR_VL53L0X)
            return _vl53l0x_getMeasurement();
        else if (_sensor_type == TOF_SENSOR_VL53l1X)
            return _vl53l1x_getMeasurement();
        else if (_sensor_type == TOF_SENSOR_VL6180X)
            return _vl6180x_getMeasurement();
    }
    return 0;
}

void tof::setDistanceMode(VL53L1X::DistanceMode distanceMode)
{
    if (_enabled && _sensor_type == TOF_SENSOR_VL53l1X)
        _vl53l1x_setDistanceMode(distanceMode);
}

void tof::setRoiSize(int x, int y)
{
    if (_enabled && _sensor_type == TOF_SENSOR_VL53l1X)
        _vl53l1x_setRoiSize(x, y);
}

// See https://www.st.com/resource/en/user_manual/dm00600212-vl53l1x-ultra-lite-driver-multiple-zone-implementation-stmicroelectronics.pdf#%5B%7B%22num%22%3A14%2C%22gen%22%3A0%7D%2C%7B%22name%22%3A%22XYZ%22%7D%2Cnull%2C732.75592%2Cnull%5D for spad_num definitions
void tof::setRoiCenter(int spad_num)
{
    if (_enabled && _sensor_type == TOF_SENSOR_VL53l1X)
        _vl53l1x_setRoiCenter(spad_num);
}

bool tof::timeoutOccurred() 
{
    if (_sensor_type == TOF_SENSOR_VL53L0X)
        return _vl53l0x_timeoutOccured(); 
    else if (_sensor_type == TOF_SENSOR_VL53l1X)
        return vl53l1x_sensor->timeoutOccurred();
    return false;
}



// VL53L0X
void tof::_vl53l0x_begin(uint8_t address = 0b0101001)
{
    log_inline("Init as VL53L0X: ");
    vl53l0x_sensor->setTimeout(500);
    if (!vl53l0x_sensor->init())
    {
        logln("Failed to detect and initialize VL53L0X!");
        this->enable(false);
        _error = TOF_ERROR_FAILED_TO_INITIALIZE;
        return;
    }
    else
    {
        logln("init successful!");
    }
    delay(10);
    changeAddress(address);
    // setHighAccuracy(false);
    // setLongRangeMode(false);
    _error = TOF_ERROR_NONE;
}

void tof::_vl53l0x_setLongRangeMode(bool mode)
{
    _long_range = mode;
    if (mode)
    {
        // lower the return signal rate limit (default is 0.25 MCPS)
        vl53l0x_sensor->setSignalRateLimit(0.1);
        // increase laser pulse periods (defaults are 14 and 10 PCLKs)
        vl53l0x_sensor->setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
        vl53l0x_sensor->setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
    }
    else
    {
        // lower the return signal rate limit (default is 0.25 MCPS)
        vl53l0x_sensor->setSignalRateLimit(0.25);
        // increase laser pulse periods (defaults are 14 and 10 PCLKs)
        vl53l0x_sensor->setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 14);
        vl53l0x_sensor->setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 10);
    }
}

void tof::_vl53l0x_setHighAccuracy(bool mode)
{
    if (mode)
    {
        _high_accuracy = true;
        _high_speed = false;
        vl53l0x_sensor->setMeasurementTimingBudget(200000);
    }
    else
    {
        _high_accuracy = false;
        _high_speed = false;
        vl53l0x_sensor->setMeasurementTimingBudget(33000);
    }
}

void tof::_vl53l0x_setHighSpeed(bool mode)
{
    if (mode)
    {
        _high_accuracy = false;
        _high_speed = true;
        vl53l0x_sensor->setMeasurementTimingBudget(20000);
    }
    else
    {
        _high_accuracy = false;
        _high_speed = false;
        vl53l0x_sensor->setMeasurementTimingBudget(33000);
    }
}

void tof::_vl53l0x_changeAddress(uint8_t address)
{
    vl53l0x_sensor->setAddress(address);
}

uint16_t tof::_vl53l0x_getMeasurement()
{
    uint16_t measurment;
    if (continuous_mode)
    {
        measurment = vl53l0x_sensor->readRangeContinuousMillimeters();
    }
    else
    {
        measurment = vl53l0x_sensor->readRangeSingleMillimeters();
    }
    last_measurement = measurment;

    if (vl53l0x_sensor->timeoutOccurred())
    {
        _error = TOF_ERROR_TIMEOUT;
    }
    else if (measurment > 8000)
    {
        _error = TOF_ERROR_MAX_DISTANCE;
    }
    else
    {
        _error = TOF_ERROR_NONE;
    }

    return measurment;// + abs(_offset_x);
}

void tof::_vl53l0x_setContinuous(bool mode, uint32_t period_ms)
{
    continuous_mode = mode;
    if (mode)
    {
        vl53l0x_sensor->startContinuous(period_ms);
    }
    else
    {
        vl53l0x_sensor->stopContinuous();
    }
}

bool tof::_vl53l0x_timeoutOccured()
{
    return vl53l0x_sensor->timeoutOccurred();
}

// VL53L1X
void tof::_vl53l1x_begin(uint8_t address)
{
    log_inline("Init as VL53L1X: ");
    vl53l1x_sensor->setTimeout(500);
    if (!vl53l1x_sensor->init())
    {
        logln("Failed to detect and initialize VL53L1X!");
        this->enable(false);
        _error = TOF_ERROR_FAILED_TO_INITIALIZE;
        return;
    }
    else
    {
        logln("init successful!");
    }
    delay(10);
    changeAddress(address);

    setDistanceMode(VL53L1X::Short);
    setContinuous(false);
    setRoiSize(4, 16);
    setRoiCenter(247);
    
    _error = TOF_ERROR_NONE;
}

void tof::_vl53l1x_changeAddress(uint8_t address)
{
    vl53l1x_sensor->setAddress(address);
}

void tof::_vl53l1x_setDistanceMode(VL53L1X::DistanceMode mode)
{
    vl53l1x_sensor->setDistanceMode(mode);
}

void tof::_vl53l1x_setRoiSize(int x, int y)
{
    vl53l1x_sensor->setROISize(x, y);
}

void tof::_vl53l1x_setRoiCenter(int spad_num)
{
    vl53l1x_sensor->setROICenter(spad_num);
}

void tof::_vl53l1x_setContinuous(bool mode, uint32_t period_ms)
{
    continuous_mode = mode;
    if (mode)
    {
        vl53l1x_sensor->setMeasurementTimingBudget(period_ms * 1000);
        vl53l1x_sensor->startContinuous(period_ms);
    }
    else
    {
        vl53l1x_sensor->setMeasurementTimingBudget(50000);
        vl53l1x_sensor->stopContinuous();
    }
}

uint16_t tof::_vl53l1x_getMeasurement()
{
    if (continuous_mode)
    {
        vl53l1x_sensor->readRangeContinuousMillimeters();
    }
    else
    {
        vl53l1x_sensor->readRangeSingleMillimeters();
    }

    uint16_t measurement = vl53l1x_sensor->ranging_data.range_mm;

    if (vl53l1x_sensor->timeoutOccurred())
    {
        _error = TOF_ERROR_TIMEOUT;
    }
    else if (vl53l1x_sensor->ranging_data.range_status != VL53L1X::RangeValid)
    {
        _error = TOF_ERROR_SYSERROR;
    }
    else if (measurement > 8000)
    {
        _error = TOF_ERROR_MAX_DISTANCE;
    }
    else
    {
        _error = TOF_ERROR_NONE;
    }

    last_measurement = measurement;

    return measurement;
}


// VL6180X
void tof::_vl6180x_begin(uint8_t address)
{
    log_inline("Init as VL6180X: ");
    if (!vl6180x_sensor->begin())
    {
        logln("Failed to detect and initialize VL6180X!");
        this->enable(false);
        _error = TOF_ERROR_FAILED_TO_INITIALIZE;
        return;
    }
    else
    {
        logln("init successful!");
    }
    delay(10);
    changeAddress(address);
    _error = TOF_ERROR_NONE;
}

void tof::_vl6180x_changeAddress(uint8_t address)
{
    vl6180x_sensor->setAddress(address);
}

void tof::_vl6180x_setContinuous(bool mode, uint32_t period_ms)
{
    if (continuous_mode != mode)
    {
        continuous_mode = mode;
        if (mode)
        {
            vl6180x_sensor->startRangeContinuous(period_ms);
        }
        else
        {
            vl6180x_sensor->stopRangeContinuous();
        }
    }
}

uint16_t tof::_vl6180x_getMeasurement()
{
    uint8_t range = 0;
    if (continuous_mode)
    {
        // vl6180x_sensor->waitRangeComplete();
        range = vl6180x_sensor->readRangeResult();
    }
    else
    {
        range = vl6180x_sensor->readRange();
    }
    
    uint8_t status = vl6180x_sensor->readRangeStatus();

    if (range > 150)
    {
        _error = TOF_ERROR_MAX_DISTANCE;
    }
    else if (status == VL6180X_ERROR_NONE)
    {
        _error = TOF_ERROR_NONE;
    }
    else if  ((status >= VL6180X_ERROR_SYSERR_1) && (status <= VL6180X_ERROR_SYSERR_5))
    {
        _error = TOF_ERROR_SYSERROR;
    }
    else if (status == VL6180X_ERROR_ECEFAIL)
    {
        _error = TOF_ERROR_ECE_ERROR;
    }
    else if (status == VL6180X_ERROR_NOCONVERGE)
    {
        _error = TOF_ERROR_NOCONVERGENCE;
    }
    else if (status == VL6180X_ERROR_RANGEIGNORE)
    {
        _error = TOF_ERROR_SNR_ERROR;
        range = 0;
    }
    else if (status == VL6180X_ERROR_SNR)
    {
        _error = TOF_ERROR_SNR_ERROR;
    }
    else if (status == VL6180X_ERROR_RAWUFLOW || status == VL6180X_ERROR_RANGEUFLOW)
    {
        _error = TOF_ERROR_MIN_DISTANCE;
    }
    else if (status == VL6180X_ERROR_RAWOFLOW || status == VL6180X_ERROR_RANGEOFLOW)
    {
        _error = TOF_ERROR_MAX_DISTANCE;
    }

    return range;
}
