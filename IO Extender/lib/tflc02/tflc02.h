#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>

class tflc02 {
    public:
        struct TF {
            int distance;
            u8 ErrorCode;
            boolean receiveComplete;
        };

        tflc02() {};
        void init();
        void tick();

        TF get_data();
    private:
        TF Lidar;
        TF lastValue;
        SoftwareSerial* SensorSerial = new SoftwareSerial(8, 9);

        void getLidarData(TF* lidar);
        void Port_Print_Ascii(TF* lidar);
        void Port_Print_Benewake_9Byte(TF* lidar);

        u8 get_distance_mm_command[5] = {0x55, 0xAA, 0x81, 0x00, 0xFA};  //Get distance command

        unsigned long last_sensor_received = 0;
};
