#ifndef ACCEL_H
#define ACCEL_H

#define ACCEL_LIBRARY 2

#include <MPU6050_light.h>

#include "logger.h"

class accel {
    public:
        accel();
        void init();
        void enable(bool enabled);
        bool is_enabled() { return _accel_enabled; }
        float get_pitch_degrees();
        float get_roll_degrees();
        float get_pitch_for_compensation();
        float get_roll_for_compensation();
        enum ramp_types {
            up,
            down,
            level
        } ;
        ramp_types getCurrentRampState();
        void print_values();
    private:
        MPU6050* mpu = new MPU6050(Wire);
        float acc_lsb_to_g = 16384.0f; // NEEDS TO BE CHANGED if mpu->setAccConfig() is called [https://github.com/rfetick/MPU6050_light/blob/master/src/MPU6050_light.cpp#L89]
        float range_per_digit = .000061f; // ---''---  [https://github.com/jarzebski/Arduino-MPU6050/blob/master/MPU6050.cpp#L107]
        
        bool _accel_enabled = false;
        float max_angle = 0.0f;
};

#endif /* ACCEL_H */
