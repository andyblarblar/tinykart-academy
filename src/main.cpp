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
    logger.print("Toggle Pause\n");

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

    // Init PWM
    analogWriteResolution(PWM_BITS); // Range of 0-4096
    analogWriteFrequency(PWM_FREQ);

    // Prepare kart for motion
    ESC esc{THROTTLE_PIN, PWM_MAX_DUTY, PWM_FREQ};
    tinyKart = new TinyKart{STEERING_PIN, esc};

    // Init DMA and UART for LiDAR
    dmaSerialRx5.init(230'400, [&](volatile LD06Buffer buffer) {
        // On each packet received, copy over to driver.
        ld06.add_buffer(buffer);
    });

    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
}

void loop() {
    noInterrupts();
    auto res = ld06.get_scan();
    interrupts();

    if (res) {
        auto scan_res = *res;

        if (scan_res) {
            logger.printf("Stamp: %u\n", scan_res.scan.timestamp);

            for (int i = 0; i < 12; i++) {
                //logger.printf("Range: %u\n", scan_res.scan.data[i].dist);
            }
        } else {
            switch (scan_res.error) {
                case ScanResult::Error::CRCFail:
                    logger.print("CRC error!\n");
                    break;

                case ScanResult::Error::HeaderByteWrong:
                    logger.print("Header byte wrong!\n");
                    break;
            }
        }
    }
}