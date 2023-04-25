#include "Wire.h"
#include "Wire1.h"

#include "Adafruit_VL6180X.h"
Adafruit_VL6180X vl6180x = Adafruit_VL6180X();

#include <MPU6050_light.h>
MPU6050 accel = MPU6050(Wire);

#define PIN_PORTA 4
#define PIN_PORTB 3
#define PIN_PORTC 2
#define PIN_PORTD 5

#define TASTER_FRONT A0
#define TASTER_BACK_RIGHT A1
#define TASTER_BACK_LEFT A2

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

enum tof_error_types {
    TOF_ERROR_NONE,
    TOF_ERROR_TIMEOUT,
    TOF_ERROR_MAX_DISTANCE,
    TOF_ERROR_NOT_ENABLED,
    TOF_ERROR_FAILED_TO_INITIALIZE,
    TOF_ERROR_MIN_DISTANCE,
    TOF_ERROR_SYSERROR,
    TOF_ERROR_ECE_ERROR,
    TOF_ERROR_NOCONVERGENCE,
    TOF_ERROR_SNR_ERROR,
};
bool tof_claw_init_success = false;
void tof_claw_init();
void tof_claw_tick();

void taster_init();
void taster_tick();

bool accel_init_success = false;
void accel_init();
void accel_tick();

void print_tx();

void wire_on_receive(int num_bytes);
void wire_on_request();

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(115200);

    tof_claw_init();

    taster_init();

    accel_init();

    Wire1.begin(0x09);
    Wire1.onReceive(wire_on_receive);
    Wire1.onRequest(wire_on_request);
}

void loop()
{
    tof_claw_tick();

    taster_tick();

    accel_tick();

    print_tx();
    delay(10);
}

void tof_claw_init()
{
    Serial.print("Init VL6180X: ");
    pinMode(PIN_PORTB, OUTPUT);
    digitalWrite(PIN_PORTB, LOW);
    delay(10);
    digitalWrite(PIN_PORTB, HIGH);
    delay(10);

    if (!vl6180x.begin())
    {
        Serial.println(F("Failed to boot VL6180X"));
    }
    else
    {
        vl6180x.setAddress(0x31);
        vl6180x.startRangeContinuous(50);

        tof_claw_init_success = true;
        Serial.println("Init of VL6180X Successful");
    }
}
void tof_claw_tick()
{
    if (tof_claw_init_success && vl6180x.isRangeComplete())
    {
        uint8_t range = vl6180x.readRangeResult();
        uint8_t status = vl6180x.readRangeStatus();

        tof_error_types _error = TOF_ERROR_NONE;

        if (range > 150)
        {
            _error = TOF_ERROR_MAX_DISTANCE;
        }
        else if (status == VL6180X_ERROR_NONE)
        {
            _error = TOF_ERROR_NONE;
        }
        else if  ((status >= VL6180X_ERROR_SYSERR_1) && (status <= VL6180X_ERROR_SYSERR_5))
        {
            _error = TOF_ERROR_SYSERROR;
        }
        else if (status == VL6180X_ERROR_ECEFAIL)
        {
            _error = TOF_ERROR_ECE_ERROR;
        }
        else if (status == VL6180X_ERROR_NOCONVERGE)
        {
            _error = TOF_ERROR_NOCONVERGENCE;
        }
        else if (status == VL6180X_ERROR_RANGEIGNORE)
        {
            _error = TOF_ERROR_SNR_ERROR;
            range = 0;
        }
        else if (status == VL6180X_ERROR_SNR)
        {
            _error = TOF_ERROR_SNR_ERROR;
        }
        else if (status == VL6180X_ERROR_RAWUFLOW || status == VL6180X_ERROR_RANGEUFLOW)
        {
            _error = TOF_ERROR_MIN_DISTANCE;
        }
        else if (status == VL6180X_ERROR_RAWOFLOW || status == VL6180X_ERROR_RANGEOFLOW)
        {
            _error = TOF_ERROR_MAX_DISTANCE;
        }

        tx.claw_tof_value = range;
        tx.claw_tof_error = _error;
    }
}

void taster_init()
{
    Serial.print("Init Taster: ");
    pinMode(TASTER_FRONT, INPUT);
    pinMode(TASTER_BACK_LEFT, INPUT);
    pinMode(TASTER_BACK_RIGHT, INPUT);
    Serial.println("Successful!");
}
void taster_tick()
{
    bool front_left = false, front_right = false, back_left, back_right;

    int front_val = analogRead(TASTER_FRONT);
    // Serial.printf("Front: %d\r\n", front_val);
    if (front_val > 856)
    {
        front_left = true;
        front_right = true;
    }
    else if (front_val > 511)
    {
        front_left = true;
        front_right = false;
    }
    else if (front_val > 160)
    {
        front_left = false;
        front_right = true;
    }

    back_left = analogRead(TASTER_BACK_LEFT) < 358;
    back_right = analogRead(TASTER_BACK_RIGHT) < 358;

    uint8_t packed_value = 0;
    packed_value |= (front_left << 3);
    packed_value |= (front_right << 2);
    packed_value |= (back_left << 1);
    packed_value |= back_right;

    tx.taster_values = packed_value;
}

void accel_init()
{
    Serial.print("Init Accel: ");
    if (accel.begin() != 0)
    {
        Serial.println("Accel initialisation failed!");
    }
    else
    {
        accel.setAccOffsets(-0.02, 0.06, 0.09);
        accel.setGyroOffsets(5.69, -0.36, -0.77);

        accel_init_success = true;
        Serial.println("Init of Accel Successful");
    }
}
void accel_tick()
{
    if (accel_init_success)
    {
        accel.update();
        tx.accel_value = accel.getAngleY();
    }
}


void print_tx()
{
    if (Serial.available() > 0)
    {
        uint8_t value = tx.taster_values;
        bool _fl_state = (value >> 3) & 0x01;
        bool _fr_state = (value >> 2) & 0x01;
        bool _bl_state = (value >> 1) & 0x01;
        bool _br_state = value & 0x01;

        Serial.printf("Accel: %d\tTof_Claw: %d(%d)\tTaster: %d%d%d%d\r\n", (int)round(tx.accel_value), tx.claw_tof_value, (int)tx.claw_tof_error, _fl_state ? 1 : 0, _fr_state ? 1 : 0, _bl_state ? 1 : 0, _br_state ? 1 : 0);
    }
}

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
