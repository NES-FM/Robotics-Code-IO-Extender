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
    if (Serial.available() > 0)
    { 
        Serial.print("Received Wire event of size: "); Serial.println(howMany); 
    }

    switch (howMany)
    {
        case sizeof(digital_write_command):
            Wire.readBytes((byte*) &_rx_digital_write_command, sizeof(digital_write_command));
            if (Serial.available() > 0)
            {
                Serial.print("digital_write_command with Pin: "); Serial.print(_rx_digital_write_command.pin); Serial.print(" and Value: "); Serial.println(_rx_digital_write_command.value);
            }
            digitalWrite(_rx_digital_write_command.pin, _rx_digital_write_command.value);
            break;

        case sizeof(read_command):
            Wire.readBytes((byte*) &_rx_read_command, sizeof(read_command));
            if (Serial.available() > 0)
            {
                Serial.print("read_command with Type: "); Serial.print(_rx_read_command.what_to_read); Serial.print(" and Pin: "); Serial.println(_rx_read_command.pin);
            }
            break;

        case sizeof(pin_mode_command):
            Wire.readBytes((byte*) &_rx_pin_mode_command, sizeof(pin_mode_command));
            if (Serial.available() > 0)
            {
                Serial.print("pin_mode_command with Pin: "); Serial.print(_rx_pin_mode_command.pin); Serial.print(" and Mode: "); Serial.println(_rx_pin_mode_command.mode);
            }
            pinMode(_rx_pin_mode_command.pin, _rx_pin_mode_command.mode);
            break;

        case sizeof(analog_write_command):
            Wire.readBytes((byte*) &_rx_analog_write_command, sizeof(analog_write_command));
            if (Serial.available() > 0)
            {
                Serial.print("analog_write_command with Pin: "); Serial.print(_rx_analog_write_command.pin); Serial.print(" and Value: "); Serial.println(_rx_analog_write_command.value);
            }
            analogWrite(_rx_analog_write_command.pin, _rx_analog_write_command.value);
            break;

        default:
            break;
    }
}

void comm::requestEvent()
{
    uint8_t what_to_read = _rx_read_command.what_to_read;
    if (what_to_read == (uint8_t)read_types::READ_TYPE_ANALOG)
    {
        _tx_analog_read_answer.value = analogRead(_rx_read_command.pin);
        Wire.write((byte*) &_tx_analog_read_answer, sizeof(_tx_analog_read_answer));
    }
    else if (what_to_read == (uint8_t)read_types::READ_TYPE_DIGITAL)
    {
        _tx_digital_read_answer.value = digitalRead(_rx_read_command.pin);
        Wire.write((byte*) &_tx_digital_read_answer, sizeof(_tx_digital_read_answer));
    }
    // else if (what_to_read == (uint8_t)read_types::READ_TYPE_LC02)
    // {
    //     tflc02::TF tflc02_value = _tflc02_ref->get_data();
    //     _tx_lc02_answer.distance = tflc02_value.distance;
    //     _tx_lc02_answer.error_code = tflc02_value.ErrorCode;
    //     Wire.write((byte*) &_tx_lc02_answer, sizeof(_tx_lc02_answer));
    // }
}

void comm::tick()
{

}
