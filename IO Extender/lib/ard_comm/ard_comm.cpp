#include "ard_comm.h"

void comm::init()
{
    Wire.begin(I2C_ADDRESS);
}

// pin_mode_command _rx_pin_mode_command;
// digital_write_command _rx_digital_write_command;
// digital_read_command _rx_digital_read_command;
// analog_read_command _rx_analog_read_command;
// analog_write_command _rx_analog_write_command;

void comm::receiveEvent(int howMany)
{
    // if (Serial.available() > 0)
    // { 
        Serial.print("Received Wire event of size: "); Serial.println(howMany); 
    // }
    INPUT;
    INPUT_PULLUP;
    OUTPUT;

    switch (howMany)
    {
        case sizeof(pin_mode_command):
            Wire.readBytes((byte*) &_rx_pin_mode_command, sizeof(pin_mode_command));
            // if (Serial.available() > 0)
            // {
                Serial.print("pin_mode_command with Pin: "); Serial.print(_rx_pin_mode_command.pin); Serial.print(" and Mode: "); Serial.println(_rx_pin_mode_command.mode);
            // }
            pinMode(_rx_pin_mode_command.pin, _rx_pin_mode_command.mode);
            break;

        case sizeof(digital_write_command):
            Wire.readBytes((byte*) &_rx_digital_write_command, sizeof(digital_write_command));
            // if (Serial.available() > 0)
            // {
                Serial.print("digital_write_command with Pin: "); Serial.print(_rx_digital_write_command.pin); Serial.print(" and Value: "); Serial.println(_rx_digital_write_command.value);
            // }
            digitalWrite(_rx_digital_write_command.pin, _rx_digital_write_command.value);
            break;

        case sizeof(digital_read_command):
            Wire.readBytes((byte*) &_rx_digital_read_command, sizeof(digital_read_command));
            // if (Serial.available() > 0)
            // {
                Serial.print("digital_read_command with Pin: "); Serial.println(_rx_digital_read_command.pin);
            // }
            break;

        case sizeof(analog_read_command):
            Wire.readBytes((byte*) &_rx_analog_read_command, sizeof(analog_read_command));
            // if (Serial.available() > 0)
            // {
                Serial.print("analog_read_command with Pin: "); Serial.println(_rx_analog_read_command.pin);
            // }
            break;

        case sizeof(analog_write_command):
            Wire.readBytes((byte*) &_rx_analog_write_command, sizeof(analog_write_command));
            // if (Serial.available() > 0)
            // {
                Serial.print("analog_write_command with Pin: "); Serial.print(_rx_analog_write_command.pin); Serial.print(" and Value: "); Serial.println(_rx_analog_write_command.value);
            // }
            analogWrite(_rx_analog_write_command.pin, _rx_analog_write_command.value);
            break;

        default:
            break;
    }    
}

void comm::tick()
{

}
