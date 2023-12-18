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

// Scan processor
ScanBuilder scan_builder{360 - 90, 90};

/// Starts/stops the kart
void estop() {
    logger.printf("Toggle Pause\n");

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
    dmaSerialRx5.begin(230'400, [&](volatile LD06Buffer buffer) {
        // On each packet received, copy over to driver.
        ld06.add_buffer(buffer, 47);
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
            auto maybe_scan = scan_builder.add_frame(scan_res.scan);

            if (maybe_scan) {
                auto scan = *maybe_scan;

                logger.printf("*****START SCAN******\n");
                for (auto &pt: scan) {
                    logger.printf("Point: (%hu,%hu)\n", (uint16_t) pt.x, (uint16_t) pt.y);
                }
                logger.printf("*****END SCAN******\n\n");

                // If object is 10cm in front of kart, stop TODO move this into auton or cleanup
                if (scan[scan.size() / 2].dist(ScanPoint{0, 0}) < 100) {
                    tinyKart->pause();
                    digitalWrite(LED_YELLOW, HIGH);
                }

                // TODO add auton here
            }
        } else {
            switch (scan_res.error) {
                case ScanResult::Error::CRCFail:
                    logger.printf("CRC error!\n");
                    break;

                case ScanResult::Error::HeaderByteWrong:
                    logger.printf("Header byte wrong!\n");
                    break;
            }
        }
    }
}