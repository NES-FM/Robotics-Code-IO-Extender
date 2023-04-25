#include <Arduino.h>

#include "i2c_addresses.h"
#include "pin_definitions.h"

#include "SPI.h"
#include "Wire.h"

#include "VL6180X.h"
VL6180X vl6180x;

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    Serial.println("Hi");
    delay(135);
}
