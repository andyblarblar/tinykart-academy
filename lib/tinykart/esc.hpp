#pragma once

#include "cassert"
#include "Arduino.h"

/// Traxxas XL5 ESC control.
///
/// This ESC is controlled like a servo, using pulse period modulation. Basically, instead of modulating duty cycle,
/// we instead modulate the period of the duty cycle. Ex. Neutral is a high period of 1.0ms per PWM signal.
///
/// For this ESC, 1.0ms is full reverse power, 1.5ms is neutral, and 2.0ms is full forward.
///
/// The ESC must also be armed before use, with a special sequence of forward and reverse signals.
class ESC {
    int pwm_pin;
    /// Max PWM register value
    int max_pwm_duty;
    /// Max PWM period in ms
    float period;
    /// Tracks if we were moving forward, for clearing locks
    bool was_forward = false;

public:
    /// Initializes an ESC.
    /// @param pwm_out_pin PWM pin to write to
    /// @param max_pwm_duty Max PWM duty register value
    /// @param frequency_set Frequency PWM pins are writing out (configured already)
    explicit ESC(int pwm_out_pin, int max_pwm_duty, float frequency_set) : pwm_pin(pwm_out_pin),
                                                                           period((1 / frequency_set) * 1000),
                                                                           max_pwm_duty(max_pwm_duty) {
        pinMode(pwm_out_pin, OUTPUT);
    }

    /// Arms the ESC. Set to neutral after exiting.
    void arm() {
        this->set_reverse_raw(1.0);
        delay(int(period * 2));

        this->set_forward(1.0);
        delay(int(period * 2));

        this->set_neutral();
        delay(600);
    }

    /// Sets the motor to neutral.
    void set_neutral() {
        // Neutral is 1.5ms periods
        auto value = uint16_t((1.5 / period) * max_pwm_duty);
        analogWrite(pwm_pin, value);
    }

    /// Sets the esc to power forward with some 0.0-1.0 percent.
    void set_forward(float power) {
        assert(power <= 1.0 && power >= 0.0);

        this->was_forward = true;

        // Forward is 1.5-2.0ms periods
        auto value = uint16_t(((1.5 + power * 0.5) / period) * max_pwm_duty);
        analogWrite(pwm_pin, value);
    }

    /// Forces the ESC to move in reverse. May not work without clearing the lockout.
    void set_reverse_raw(float power) {
        assert(power <= 1.0 && power >= 0.0);

        // Reverse is 1.0-1.5ms periods
        auto value = uint16_t(((1.5 - power * 0.5) / period) * max_pwm_duty);
        analogWrite(pwm_pin, value);
    }

    /// Sets the esc to power reverse with some 0.0-1.0 percent.
    void set_reverse(float power) {
        assert(power <= 1.0 && power >= 0.0);

        // Clear lockout
        if (this->was_forward) {
            this->set_reverse_raw(0.4);
            delay(uint16_t(period * 2));

            this->set_neutral();
            delay(uint16_t(period * 4));
            this->was_forward = false;
        }

        this->set_reverse_raw(power);
    }
};