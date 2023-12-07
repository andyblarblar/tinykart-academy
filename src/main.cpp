#include <Arduino.h>
#include "pins.hpp"
#include "HardwareSerial.h"

void setup() {
    // Simply changes PIN modes, does not setup perfs
    tk::setup_pins();

    // Connect to PC for logging
    Serial.begin(9800);

    // Init PC serial
    while (!Serial.availableForWrite()) {
        digitalToggle(LED_RED);
        delay(20);
    }
    digitalWrite(LED_RED, LOW);

    // Init UART
    Serial1.begin(230400, SERIAL_8N1); //TODO replace with LiDAR driver? Look into enabling DMA

    // Init PWM
    analogWriteResolution(12); // Range of 0-4092
    analogWriteFrequency(100);
}

void loop() {
}