#include "tflc02.h"

void tflc02::init()
{
    SensorSerial->begin(115200);

    lastValue.distance = 0;
    lastValue.ErrorCode = 0;
    lastValue.receiveComplete = true;
}

tflc02::TF tflc02::get_data()
{
    return lastValue;
}

void tflc02::tick()
{
    if ((millis() - last_sensor_received) >= 33 && !Lidar.receiveComplete)
    {
        SensorSerial->write(get_distance_mm_command, 5);
    } 
    else if (Lidar.receiveComplete)
    {
        // Debug Options
        // Port_Print_Ascii(&Lidar);         
        // Port_Print_Benewake_9Byte(&Lidar);

        lastValue.distance = Lidar.distance;
        lastValue.ErrorCode = Lidar.ErrorCode;

        Lidar.receiveComplete = false;

        // delay(33); // Replaced with millis() timer
        last_sensor_received = millis();
    }

    if(SensorSerial->available() > 0)
    {
        getLidarData(&Lidar);
    }
}

void tflc02::getLidarData(TF* lidar) {
    static char i = 0;
    static int rx[8];
    while (SensorSerial->available())
    {
        rx[i] = SensorSerial->read();
        if (rx[0] != 0x55)
        {
        i = 0;
        } else if (i == 1 && rx[1] != 0xAA)
        {
        i = 0;
        } else if (i == 7)
        {
        i = 0;
        if (rx[7] == 0xFA)
        {
            lidar->distance = rx[5] + rx[4] * 256;
            lidar->ErrorCode  = rx[6];
            lidar->receiveComplete = true;
        }
        } else
        {
        i++;
        }
    }
}

void tflc02::Port_Print_Ascii(TF* lidar)
{
    Serial.print("Dist = ");
    Serial.print(lidar->distance);
    if(lidar->ErrorCode)
    {
        Serial.print("\tErrorCode = ");
        Serial.print(lidar->ErrorCode,HEX);
    }
    Serial.println();
}

void tflc02::Port_Print_Benewake_9Byte(TF* lidar) {
    u8 i = 0;
    u8 CheckSum = 0;
    u8 B_9Byte[9];
    B_9Byte[0] = 0x59;
    B_9Byte[1] = 0x59;
    B_9Byte[2] = lidar -> distance & 0xFF;
    B_9Byte[3] = lidar -> distance >> 8;
    B_9Byte[4] = 0x00;
    B_9Byte[5] = 0x00;
    B_9Byte[6] = 0x00;
    B_9Byte[7] = 0x00;
    for (i = 0; i < 7; i++) {
        CheckSum += B_9Byte[i];
    }
    B_9Byte[8] = CheckSum & 0xFF;
    Serial.write(B_9Byte, 9);
}
