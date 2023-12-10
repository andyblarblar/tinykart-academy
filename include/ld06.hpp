#pragma once

#include "optional"

static const uint8_t CrcTable[256] =
        {
                0x00, 0x4d, 0x9a, 0xd7, 0x79, 0x34, 0xe3,
                0xae, 0xf2, 0xbf, 0x68, 0x25, 0x8b, 0xc6, 0x11, 0x5c, 0xa9, 0xe4, 0x33,
                0x7e, 0xd0, 0x9d, 0x4a, 0x07, 0x5b, 0x16, 0xc1, 0x8c, 0x22, 0x6f, 0xb8,
                0xf5, 0x1f, 0x52, 0x85, 0xc8, 0x66, 0x2b, 0xfc, 0xb1, 0xed, 0xa0, 0x77,
                0x3a, 0x94, 0xd9, 0x0e, 0x43, 0xb6, 0xfb, 0x2c, 0x61, 0xcf, 0x82, 0x55,
                0x18, 0x44, 0x09, 0xde, 0x93, 0x3d, 0x70, 0xa7, 0xea, 0x3e, 0x73, 0xa4,
                0xe9, 0x47, 0x0a, 0xdd, 0x90, 0xcc, 0x81, 0x56, 0x1b, 0xb5, 0xf8, 0x2f,
                0x62, 0x97, 0xda, 0x0d, 0x40, 0xee, 0xa3, 0x74, 0x39, 0x65, 0x28, 0xff,
                0xb2, 0x1c, 0x51, 0x86, 0xcb, 0x21, 0x6c, 0xbb, 0xf6, 0x58, 0x15, 0xc2,
                0x8f, 0xd3, 0x9e, 0x49, 0x04, 0xaa, 0xe7, 0x30, 0x7d, 0x88, 0xc5, 0x12,
                0x5f, 0xf1, 0xbc, 0x6b, 0x26, 0x7a, 0x37, 0xe0, 0xad, 0x03, 0x4e, 0x99,
                0xd4, 0x7c, 0x31, 0xe6, 0xab, 0x05, 0x48, 0x9f, 0xd2, 0x8e, 0xc3, 0x14,
                0x59, 0xf7, 0xba, 0x6d, 0x20, 0xd5, 0x98, 0x4f, 0x02, 0xac, 0xe1, 0x36,
                0x7b, 0x27, 0x6a, 0xbd, 0xf0, 0x5e, 0x13, 0xc4, 0x89, 0x63, 0x2e, 0xf9,
                0xb4, 0x1a, 0x57, 0x80, 0xcd, 0x91, 0xdc, 0x0b, 0x46, 0xe8, 0xa5, 0x72,
                0x3f, 0xca, 0x87, 0x50, 0x1d, 0xb3, 0xfe, 0x29, 0x64, 0x38, 0x75, 0xa2,
                0xef, 0x41, 0x0c, 0xdb, 0x96, 0x42, 0x0f, 0xd8, 0x95, 0x3b, 0x76, 0xa1,
                0xec, 0xb0, 0xfd, 0x2a, 0x67, 0xc9, 0x84, 0x53, 0x1e, 0xeb, 0xa6, 0x71,
                0x3c, 0x92, 0xdf, 0x08, 0x45, 0x19, 0x54, 0x83, 0xce, 0x60, 0x2d, 0xfa,
                0xb7, 0x5d, 0x10, 0xc7, 0x8a, 0x24, 0x69, 0xbe, 0xf3, 0xaf, 0xe2, 0x35,
                0x78, 0xd6, 0x9b, 0x4c, 0x01, 0xf4, 0xb9, 0x6e, 0x23, 0x8d, 0xc0, 0x17,
                0x5a, 0x06, 0x4b, 0x9c, 0xd1, 0x7f, 0x32, 0xe5, 0xa8
        };

uint8_t CalCRC8(uint8_t *p, uint8_t len) {
    uint8_t crc = 0;
    uint16_t i;
    for (i = 0; i < len; i++) {
        crc = CrcTable[(crc ^ *p++) & 0xff];
    }
    return crc;
}

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

class LD06 {
    uint8_t current_scan[47];
    int8_t left_in_current_scan = 47;
    uint8_t next_scan[47];
    int8_t left_in_next_scan = 47;

    std::optional<LD06Frame> process_buffer() {
        // Check if header is first byte
        if (current_scan[0] != 0x54) {
            Serial.println("Scan misaligned!");
            return std::nullopt;
        }

        // Check for errors
        if (CalCRC8(current_scan, 47) != 0) {
            Serial.println("CRC Failed!");
            return std::nullopt;
        }

        LD06Frame packet{};

        // Begin reads
        packet.radar_speed = reinterpret_cast<uint16_t *>(current_scan)[2];
        packet.start_angle = float(reinterpret_cast<uint16_t *>(current_scan)[4]) / 100;

        // Read ranges (indexes 6-41)
        for (int i = 6, j = 0; i < 42; i += 3, j++) {
            packet.data[j].dist = reinterpret_cast<uint16_t *>(current_scan)[i];
            packet.data[j].confidence = current_scan[i + 2];
        }

        packet.end_angle = float(reinterpret_cast<uint16_t *>(current_scan)[42]) / 100;
        packet.timestamp = reinterpret_cast<uint16_t *>(current_scan)[44];
        packet.crc8 = current_scan[46];

        // Swap buffers
        memcpy((void *) current_scan, (void *) next_scan, 47);
        left_in_current_scan = left_in_next_scan;
        left_in_next_scan = 47;

        return packet;
    }

public:
    void add_buffer(uint8_t *buffer) {
        if (left_in_current_scan == 0 && left_in_next_scan == 0) {
            Serial.println("Dropping scan!");
            return;
        }

        // Explicitly align if new scan
        if (left_in_current_scan == 47) {
            // Find header byte
            uint8_t header_idx = 0;
            for (uint8_t i = 0; i < 47; i++) {
                if (buffer[i] == 0x54) {
                    header_idx = i;
                    break;
                }
            }

            // Only copy header onwards
            memcpy((void *) current_scan, (void *) (buffer + header_idx), 47 - header_idx);
            left_in_current_scan = 47 - (47 - header_idx);
            return;
        }

        // Room left in current scan
        if (left_in_current_scan != 0) {
            auto next_idx = 47 - left_in_current_scan - 1;

            auto current_len = 47 - left_in_current_scan;
            memcpy((void *) (current_scan + next_idx), (void *) buffer, current_len);
            left_in_current_scan = 0;

            // TODO very sus math here
            memcpy((void *) next_scan, (void *) (buffer + (47 - current_len - 1)), 47 - current_len);
            left_in_next_scan = 47 - (47 - current_len);
        }

        // TODO write into next when current full
    }

    std::optional<LD06Frame> get_scan() {
        auto res = this->process_buffer();
        // TODO finish
    }
};