#include <Arduino.h>

#include "ard_comm.h"
comm comunication; 
void wire_receive_event(int num_bytes) { comunication.receiveEvent(num_bytes); }

void setup() {
    Serial.begin(115200);

    comunication.init();
    Wire.onReceive(wire_receive_event);

    Serial.println("Everything is Started up!");
}

void loop() {
    delay(10);
}