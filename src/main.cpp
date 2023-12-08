#include <Arduino.h>
#include "pins.hpp"
#include "esc.hpp"
#include "kart.hpp"
#include "ld06.hpp"

// Robot control
TinyKart *tinyKart;

// LiDAR
LD06 *ld06;

/// Starts/stops the kart
void estop() {
    Serial.println("Toggle Pause");

    tinyKart->toggle_pause();
    digitalToggle(LED_YELLOW);
}

void setup() {
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

    // Init Lidar
    ld06 = new LD06{Serial5, LIDAR_PIN};

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
    auto scan = ld06->update();

    if (scan) {
        Serial.printf("Scan start angle: %i \n", (int) (*scan).start_angle);
    }
}