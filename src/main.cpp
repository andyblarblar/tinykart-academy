#include <Arduino.h>
#include "pins.hpp"
#include "esc.hpp"
#include "kart.hpp"
#include "ld06.hpp"
#include "dma.hpp"
#include "logger.hpp"

// Robot control
TinyKart *tinyKart;

// LiDAR
LD06 ld06{};

/// Starts/stops the kart
void estop() {
    logger.println("Toggle Pause");

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

    pinMode(LIDAR_PIN, INPUT);

    // Init PWM
    analogWriteResolution(PWM_BITS); // Range of 0-4096
    analogWriteFrequency(PWM_FREQ);

    // Prepare kart for motion
    ESC esc{THROTTLE_PIN, PWM_MAX_DUTY, PWM_FREQ};
    tinyKart = new TinyKart{STEERING_PIN, esc};

    // Init DMA and UART for LiDAR
    dmaSerialRx5.begin(230'400, [&](LD06Buffer buffer) {
        // On each packet received, copy over to driver.
        // NOTE: this code runs in the DMA IRQ, so edit with care.
        ld06.add_buffer(buffer);
    });

    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
}

void loop() {
    //TODO try to get logger working as a first step, I think the pins arnt enabled
    logger.println("LOOOOP");

    noInterrupts();
    auto res = ld06.get_scan();
    interrupts();

    if (res) {
        digitalToggle(LED_RED);
        logger.println("Got scan!");
    }
}