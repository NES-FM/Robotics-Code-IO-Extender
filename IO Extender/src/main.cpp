#include <Arduino.h>

#include "ard_comm.h"
comm comunication; 
void wire_receive_event(int num_bytes) { comunication.receiveEvent(num_bytes); }
void wire_request_event() { comunication.requestEvent(); }

#include "tflc02.h"
tflc02 tof_sensor;

void setup() {
    Serial.begin(115200);

    // tof_sensor.init();

    comunication.init();
    comunication.set_tflc02_ref(&tof_sensor);

    Wire.onReceive(wire_receive_event);
    Wire.onRequest(wire_request_event);

    Serial.println("Everything is Started up!");
}

void loop() {
    // tof_sensor.tick();
    // delay(10);
}