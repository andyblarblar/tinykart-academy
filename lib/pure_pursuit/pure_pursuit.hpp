#pragma once

#include "kart.hpp"
#include "ld06.hpp"

struct AckermannCommand {
    float throttle_percent;
    /// Ackermann angle to set to, in degrees, left positive
    float steering_angle;
};


/// Calculates the command to move the kart to some target point.
inline AckermannCommand calculate_command_to_point(const TinyKart *const tinyKart, const ScanPoint &target_point) {
    AckermannCommand command{};

    // Calculate the angle between the robot and the goal
    float alpha = atan2f(target_point.y, target_point.x);

    // Set the desired steering angle and set it to the ackerman command
    float steering_angle = atanf(
            2.0f * tinyKart->get_wheelbase() * sinf(alpha) / target_point.dist(ScanPoint::zero()));

    // Convert from rad to deg
    steering_angle = steering_angle * 180.0f / float(M_PI);

    // Clamp to possible angles
    steering_angle = std::clamp(steering_angle, -tinyKart->get_max_steering(), tinyKart->get_max_steering());
    command.steering_angle = steering_angle;

    // Set speed proportional to angle
    float set_speed = 1.0f - (abs(steering_angle) / tinyKart->get_max_steering());
    command.throttle_percent = std::clamp(set_speed, 0.2f, 1.0f);

    return command;
}