#pragma once
#include <Arduino.h>
#include <Wire.h>

#define I2C_ADDRESS 0x09

class comm
{
    public:
        comm() {};
        void init();
        void tick();
        void receiveEvent(int howMany);
    private:
        struct digital_read_command {
            uint8_t pin;              // + 1 byte
                                      // = 1 byte
        };

        struct digital_write_command {
            uint8_t pin;              // + 1 byte
            uint8_t value;            // + 1 byte
                                      // = 2 bytes
        };

        struct pin_mode_command {
            uint8_t pin;              // + 1 byte
            uint8_t mode;             // + 1 byte
            byte padding[1];          // + 1 byte
                                      // = 3 bytes
        };

        struct analog_read_command {
            uint8_t pin;              // + 1 byte
            byte padding[3];          // + 3 bytes
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
        digital_read_command _rx_digital_read_command;
        analog_read_command _rx_analog_read_command;
        analog_write_command _rx_analog_write_command;

        // enum received_type {
        //     received_none,
        //     received_stop,
        //     received_drive,
        //     received_move_steps
        // };

        // received_type _rx_received_type = received_none;
};
