#pragma once

#include "optional"
#include "cstdint"
#include "cstring"

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