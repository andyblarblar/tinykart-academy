#pragma once

#include "optional"
#include "tuple"
#include "vector"
#include "cstdint"
#include "cstring"
#include "cassert"
#include "cmath"

/// A range reading
struct Range {
    /// The distance from the unit, in mm.
    uint16_t dist;
    /// The intensity of the scan. 200 is 'average'.
    uint8_t confidence;
};

/// A single scan packet from the Lidar.
///
/// All angles are with clockwise respect to the arrow on the top of the unit.
struct LD06Frame {
    /// The rotational speed of the unit, in degrees per second.
    uint16_t radar_speed;
    /// The starting angle of this scan, in degrees.
    float start_angle;
    /// Distance readings.
    Range data[12];
    /// The ending angle of this scan, in degrees.
    float end_angle;
    /// The timestamp of this scan, in ms. This will roll over at 30000.
    uint16_t timestamp;
    /// The CRC check from the lidar.
    uint8_t crc8;

    /// Gets the angular step per range reading.
    [[nodiscard]] float get_step() const {
        // This is all to get the mod as an int to avoid floating point errors
        auto diff = (uint32_t(end_angle * 100.0) + 36000 - uint32_t(start_angle * 100.0)) % 36000;
        return float(diff / (12 - 1)) / 100.0f;
    }

    [[nodiscard]] float get_angle_of_reading(uint8_t reading_idx) const {
        assert(reading_idx < 12);
        auto angle = start_angle + get_step() * float(reading_idx);

        if (angle > -360.0) {
            angle -= 360.0;
        }
        return angle;
    }

    /// Translates the range from polar coordinates in terms of the LiDAR to polar coordinates in the standard format.
    /// In practice, this is rotating all points by 90 degrees clockwise, then reflecting.
    /// All angles are still in degrees.
    [[nodiscard]] std::tuple<float, float> get_range_in_polar(uint8_t reading_idx) const {
        auto range = float(data[reading_idx].dist);
        auto angle = get_angle_of_reading(reading_idx);

        auto p_deg = 90.0 - angle;
        if (p_deg < 0.0) {
            while (p_deg < 0.0) {
                p_deg += 360.0;
            }
        }
        return std::make_tuple(range, p_deg);
    }
};

typedef uint8_t LD06Buffer[47];

/// Result of a scan read.
struct ScanResult {
    enum class Error : int8_t {
        None = 1,
        CRCFail = -1,
        HeaderByteWrong = -2,
    };

    Error error = Error::None;
    LD06Frame scan{};

    explicit operator bool() const {
        return error == Error::None;
    }

    ScanResult() = default;

    explicit ScanResult(LD06Frame scan) : scan(scan), error(Error::None) {};

    explicit ScanResult(Error err) : error(err) {};
};

/// LD06 LiDAR driver.
///
/// To use this driver, a separate mechanism for actually reading the LiDAR packets must be setup. Then, these packets
/// should be copied over to the driver via add_buffer. Finally, get_scan can be called to see if a scan has been read
/// and checked.
class LD06 {
    uint8_t current_scan[47]{};
    int8_t left_in_current_scan = 47;
    uint8_t next_scan[47]{};
    int8_t left_in_next_scan = 47;

    ScanResult process_buffer();

public:
    /// Adds a scan buffer to the driver. This could be from ex. UART or DMA.
    /// Scan frame fragmentation will be handled in the driver.
    ///
    /// This function is not threadsafe with get_scan.
    void add_buffer(volatile LD06Buffer buffer, uint8_t len);

    /// Returns a scan, if one is available.
    ///
    /// This function is not threadsafe with add_buffer, so it is common to wrap this in either a mutex in an OS context
    /// or noInterrupts otherwise.
    std::optional<ScanResult> get_scan();

    /// Returns true if a scan packet can be processed.
    [[nodiscard]] bool has_scan() const;

    /// Returns true if the drivers buffer is full, and will drop any new scans unless read.
    [[nodiscard]] bool will_drop() const;
};

struct ScanPoint {
    /// Forward axis (mm)
    float x;
    /// Left positive axis (mm)
    float y;

    /// Distance in mm between two points
    [[nodiscard]] float dist(const ScanPoint &other) const {
        return std::hypot(x - other.x, y - other.y);
    }
};

/// Filters individual scan frames into larger regions.
class ScanBuilder {
    float start;
    float end;
    std::vector<ScanPoint> buffer{};
    bool last_scan_in_bounds = false;

    [[nodiscard]] bool scan_in_range(float scan_start, float scan_end) const {
        // The only valid config where end < start is split over 360/0
        if (end < start) {
            // If either start or end lands in the range, then that means some part of the scan is in the region we want
            return (scan_start >= start && scan_start <= 360) || (scan_start <= end && scan_start >= 0) ||
                   (scan_end >= start && scan_end <= 360) || (scan_end <= end && scan_end >= 0);
        } else {
            return (scan_start >= start && scan_start <= end) ||
                   (scan_end >= start && scan_end <= end);
        }
    }

public:

    /// Creates filter. Angles are in degrees, where 360 is forwards.
    /// \param start Start angle of filtered area
    /// \param end End angle of filtered area
    explicit ScanBuilder(float start, float end) : start(start), end(end) {};

    /// Adds a frame to the scan builder
    std::optional<std::vector<ScanPoint>> add_frame(const LD06Frame &frame) {
        // Filter to in range
        if (scan_in_range(frame.start_angle, frame.end_angle)) {
            last_scan_in_bounds = true;

            // Convert points to cartiesian points
            for (int i = 0; i < 12; ++i) {
                auto [range, angle] = frame.get_range_in_polar(i);
                float radian_angle = angle * (float(M_PI) / 180);

                float x = range * sinf(radian_angle);
                float y = -(range * cosf(radian_angle));

                buffer.push_back(ScanPoint{x, y});
            }
        }
            // Full scan area covered
        else if (last_scan_in_bounds) {
            last_scan_in_bounds = false;

            std::vector<ScanPoint> out{buffer};
            buffer.clear();

            return std::move(out);
        }

        return std::nullopt;
    }
};