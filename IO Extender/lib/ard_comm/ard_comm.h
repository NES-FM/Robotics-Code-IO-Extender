#pragma once
#include <Arduino.h>
#include <Wire.h>

#include "tflc02.h"

#define I2C_ADDRESS 0x09

class comm
{
    public:
        comm() {};
        void init();
        void tick();
        void receiveEvent(int howMany);
        void requestEvent();
        void set_tflc02_ref(tflc02* tflc02) { _tflc02_ref = tflc02; }
    private:
        tflc02* _tflc02_ref;

        struct digital_write_command {
            uint8_t pin;              // + 1 byte
            uint8_t value;            // + 1 byte
                                      // = 2 bytes
        };

        struct read_command {
            uint8_t what_to_read;     // + 1 byte
            uint8_t pin;              // + 1 byte
            byte padding[1];          // + 1 byte
                                      // = 3 bytes
        };

        struct pin_mode_command {
            uint8_t pin;              // + 1 byte
            uint8_t mode;             // + 1 byte
            byte padding[2];          // + 2 byte
                                      // = 4 bytes
        };

        struct analog_write_command {
            uint8_t pin;              // + 1 byte
            uint8_t value;            // + 1 byte
            byte padding[3];          // + 3 bytes
                                      // = 5 bytes
        };

        pin_mode_command _rx_pin_mode_command;
        digital_write_command _rx_digital_write_command;
        analog_write_command _rx_analog_write_command;
        read_command _rx_read_command;

        enum read_types
        {
            READ_TYPE_ANALOG = 0,
            READ_TYPE_DIGITAL = 1,
            READ_TYPE_LC02 = 2
        };

        struct analog_read_answer
        {
            uint16_t value;
        };
        
        struct digital_read_answer
        {
            uint8_t value;
        };

        struct lc02_answer
        {
            uint32_t distance;
            uint8_t error_code;
        };

        analog_read_answer _tx_analog_read_answer;
        digital_read_answer _tx_digital_read_answer;
        lc02_answer _tx_lc02_answer;
};
