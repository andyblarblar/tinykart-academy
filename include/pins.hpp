#pragma once

#include "Arduino.h"

/// Steering PWM
constexpr int STEERING_PIN = PE_9; // Actually A1
/// Throttle PWM
constexpr int THROTTLE_PIN = PA_0; // Actually D0
/// LiDAR UART
constexpr int LIDAR_PIN = PB_5; // Actually D11 (Tx on PC_12)

/// PWM frequency in hz
constexpr auto PWM_FREQ = 100;

/// PWM resolution in bits
constexpr auto PWM_BITS = 12;
/// Value for max duty cycle in PWM
constexpr auto PWM_MAX_DUTY = 1 << PWM_BITS;