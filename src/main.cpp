#include <Arduino.h>

#include "Wire.h"
#include "Wire1.h"

#include "SPI.h" // Random Bugfix

#include "i2c_addresses.h"
#include "pin_definitions.h"

#include "logger.h"
#include "accel.h"
#include "tof.h"
#include "i2c_scanner.h"

accel accel_sens;
float accel_value;

tof claw_tof(TOF_SENSOR_VL6180X, 0, 0, 0, PIN_PORTA);
float claw_tof_value;

void setup() 
{    
    Serial.begin(115200);
    Wire.begin();

    logger_init();


    // Tof X-Shut Init
    claw_tof.init_xshut();
    claw_tof.holdReset();

    delay(5);


    // I2C Enable
    scan_i2c_addresses();
    print_i2c_addresses();

    accel_sens.enable(check_device_enabled(I2C_ADDRESS_ACCELEROMETER, "accelerometer", "AC"));

    
    // Init Sensors
    accel_sens.init();

    claw_tof.releaseReset();
    claw_tof.enable(scan_single_device_enabled(I2C_ADDRESS_TOF_DEFAULT, "tof_claw_default", ""));
    claw_tof.init(I2C_ADDRESS_TOF_CLAW);
    claw_tof.enable(scan_single_device_enabled(I2C_ADDRESS_TOF_CLAW, "tof_claw", "TC"));
    claw_tof.setContinuous(true, 10);

}

void loop() {
    accel_value = accel_sens.get_roll_degrees();
    
    if (claw_tof.vl6180x_sensor->isRangeComplete())
    {
        claw_tof_value = claw_tof.getMeasurement();
    }
}