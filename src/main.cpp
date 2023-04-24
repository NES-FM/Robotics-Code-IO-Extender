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

#define USE_WIRE1_AS_COMM
//#define USE_UART_AS_COMM

accel accel_sens;
float accel_value;

tof claw_tof(TOF_SENSOR_VL6180X, 0, 0, 0, PIN_PORTA);
uint16_t claw_tof_value;
tof::tof_error_types claw_tof_error;

uint8_t get_taster_values();
uint8_t taster_values;

#pragma pack(1)
struct send_struct
{
    float accel_value;
    
    uint16_t claw_tof_value;
    uint8_t claw_tof_error;

    uint8_t taster_values;
};
volatile send_struct tx;
#pragma pack()

#ifdef USE_WIRE1_AS_COMM
void wire_on_receive(int num_bytes);
void wire_on_request();
#endif


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


    // Init Comm
    #ifdef USE_WIRE1_AS_COMM
    Wire1.begin(0x09);
    Wire1.onReceive(wire_on_receive);
    Wire1.onRequest(wire_on_request);
    #endif
}

void loop() {
    accel_value = accel_sens.get_roll_degrees();
    tx.accel_value = accel_value;
    
    if (claw_tof.vl6180x_sensor->isRangeComplete())
    {
        claw_tof_value = claw_tof.getMeasurement();
        claw_tof_error = claw_tof.getMeasurementError();

        tx.claw_tof_value = claw_tof_value;
        tx.claw_tof_error = claw_tof_error;
    }

    taster_values = get_taster_values();
    tx.taster_values = taster_values;

    logln("Accel: %.3f VL: %d Taster:%d\r\n", tx.accel_value, tx.claw_tof_value, tx.taster_values);

    delay(10);
}

uint8_t get_taster_values()
{
    bool front_left, front_right, back_left, back_right;

    int front_val = analogRead(TASTER_FRONT);
    if (front_val > 850)
    {
        front_left = true;
        front_right = true;
    }
    else if (front_val > 500)
    {
        front_left = true;
        front_right = false;
    }
    else if (front_val > 160)
    {
        front_left = false;
        front_right = false;
    }

    back_left = analogRead(TASTER_BACK_LEFT) > 500;
    back_right = analogRead(TASTER_BACK_RIGHT) > 500;

    uint8_t packed_value = 0;
    packed_value |= (front_left << 3);
    packed_value |= (front_right << 2);
    packed_value |= (back_left << 1);
    packed_value |= back_right;

    return packed_value;
}

#ifdef USE_WIRE1_AS_COMM
void wire_on_receive(int num_bytes)
{
    // dump the data
    while(Wire1.available() > 0) {
        Wire1.read();
    }
}

void wire_on_request()
{
    Wire1.write((byte*) &tx, sizeof(tx));
}
#endif
