#include "accel.h"

accel::accel() {};

void accel::init()
{
    if (_accel_enabled)
    {
        if (mpu->begin() != 0) {
            logln("Accel initialisation failed!");
            this->enable(false);
            return;
        }
        mpu->setAccOffsets(-0.02, 0.06, 0.09);
        mpu->setGyroOffsets(5.69, -0.36, -0.77);
        // mpu->calcOffsets();
        // this->print_values();
    }
}

void accel::enable(bool enabled)
{
    _accel_enabled = enabled;
}

float accel::get_pitch_degrees()
{
    if (_accel_enabled)
    {
        mpu->update();
        return mpu->getAngleX();
    }
    return 0.0f;
}

float accel::get_roll_degrees()
{
    if (_accel_enabled)
    {
        mpu->update();
        // print_values();
        
        return mpu->getAngleY();
    }
    return 0.0f;
}

float accel::get_pitch_for_compensation()
{
    return 0.0f;
}

float accel::get_roll_for_compensation()
{
    return 0.0f;
}

accel::ramp_types accel::getCurrentRampState()
{
    float angle = this->get_roll_degrees();
    if (angle < -10)
        return up;
    else if (angle > 10)
        return down;
    return level;
}

void accel::print_values()
{
    // mpu->update();
    logln("ACCELERO  X: %f\tY: %f\tZ: %f", mpu->getAccX(), mpu->getAccY(), mpu->getAccZ());

    logln("GYRO      X: %f\tY: %f\tZ: %f", mpu->getGyroX(), mpu->getGyroY(), mpu->getGyroZ());

    logln("ACC ANGLE X: %f\tY: %f", mpu->getAccAngleX(), mpu->getAccAngleY());

    logln("ANGLE     X: %f\tY: %f", mpu->getAngleX(), mpu->getAngleY());

    logln("RAW       X: %f\tY: %f", acc_lsb_to_g * (mpu->getAccX() + mpu->getAccXoffset()), acc_lsb_to_g * (mpu->getAccY() + mpu->getAccYoffset()));

    logln("OFFSET-A  X: %f\tY: %f\tZ: %f", mpu->getAccXoffset(), mpu->getAccYoffset(), mpu->getAccZoffset());

    logln("OFFSET-G  X: %f\tY: %f\tZ: %f", mpu->getGyroXoffset(), mpu->getGyroYoffset(), mpu->getGyroZoffset());

    logln("\r\n=====================================================\r\n");
}
