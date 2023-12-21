#include <Arduino.h>
#include "pins.hpp"
#include "kart.hpp"
#include "ld06.hpp"
#include "dma.hpp"
#include "logger.hpp"
#include "pure_pursuit.hpp"
#include "f1tenth_gap_follow.hpp"
#include "naive_gap_follow.hpp"

// Robot control
TinyKart *tinyKart;

// LiDAR
LD06 ld06{};

// Scan processor
ScanBuilder scan_builder{360 - 90, 90, ScanPoint{0.1524, 0}};

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
    tinyKart = new TinyKart{STEERING_PIN, esc, 0.3, 4.5};

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

    // Check if we have a scan frame
    if (res) {
        auto scan_res = *res;

        // Check if frame erred
        if (scan_res) {
            auto maybe_scan = scan_builder.add_frame(scan_res.scan);

            // Check if we have a 180 degree scan built
            if (maybe_scan) {
                auto scan = *maybe_scan;

                auto front_obj_dist = scan[scan.size() / 2].dist(ScanPoint::zero());

                // If object is 45cm in front of kart, stop (0.0 means bad point)
                if (front_obj_dist != 0.0 && front_obj_dist < 0.45 + 0.1524) {
                    tinyKart->pause();
                    digitalWrite(LED_YELLOW, HIGH);
                }

                // Find target point
                auto maybe_target_pt = gap_follow::find_gap_bubble(scan, 1.0);

                if (maybe_target_pt) {
                    auto target_pt = *maybe_target_pt;

                    logger.printf("Target point: (%hi,%hi)\n", (int16_t) (target_pt.x * 1000),
                                  (int16_t) (target_pt.y * 1000));

                    // Find command to drive to point
                    auto command = pure_pursuit::calculate_command_to_point(tinyKart, target_pt, 1.0);

                    // Set throttle proportional to distance to point in front of kart
                    command.throttle_percent = mapfloat(front_obj_dist, 0.1, 10.0, 0.15, tinyKart->get_speed_cap());

                    logger.printf("Command: throttle %hu, angle %hi\n", (uint16_t) (command.throttle_percent * 100),
                                  (int16_t) (command.steering_angle));

                    // Actuate kart
                    tinyKart->set_forward(command.throttle_percent);
                    tinyKart->set_steering(command.steering_angle);
                }
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