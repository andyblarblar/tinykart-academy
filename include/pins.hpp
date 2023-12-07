#pragma once

#include "Arduino.h"

/// Steering PWM
constexpr int STEERING_PIN = PE_9; // Actually A1
/// Throttle PWM
constexpr int THROTTLE_PIN = PA_0; // Actually D0
/// LiDAR UART
constexpr int LIDAR_PIN = PB_15;

namespace tk {
    /// Configures pin modes
    void setup_pins() {
        // Perfs
        pinMode(STEERING_PIN, OUTPUT);
        pinMode(THROTTLE_PIN, OUTPUT);
        pinMode(LIDAR_PIN, INPUT);

        // LEDs
        pinMode(LED_GREEN, OUTPUT);
        pinMode(LED_RED, OUTPUT);
        pinMode(LED_YELLOW, OUTPUT);
    }
}