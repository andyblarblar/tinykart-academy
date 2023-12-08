#include <Arduino.h>
#include "pins.hpp"
#include "esc.hpp"
#include "kart.hpp"

// Robot control
TinyKart *tinyKart;

/// Starts/stops the kart
void estop() {
    Serial.println("Toggle Pause");

    tinyKart->toggle_pause();
    digitalToggle(LED_YELLOW);
}

void setup() {
    pinMode(LIDAR_PIN, INPUT); //TODO move to lidar driver

    // LEDs
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);

    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_YELLOW, HIGH);

    // Setup blue user button on the board to stop the kart
    pinMode(USER_BTN, INPUT);
    attachInterrupt(digitalPinToInterrupt(USER_BTN), estop, FALLING);

    // Connect to PC for logging
    Serial.begin(9600);

    // Init PC serial
    while (!Serial.availableForWrite()) {
        delay(20);
    }

    // Init UART
    Serial1.begin(230400, SERIAL_8N1); //TODO replace with LiDAR driver? Look into enabling DMA

    // Init PWM
    analogWriteResolution(PWM_BITS); // Range of 0-4096
    analogWriteFrequency(PWM_FREQ);

    // Prepare kart for motion
    ESC esc{THROTTLE_PIN, PWM_MAX_DUTY, PWM_FREQ};
    tinyKart = new TinyKart{STEERING_PIN, esc};

    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
}

void loop() {
}