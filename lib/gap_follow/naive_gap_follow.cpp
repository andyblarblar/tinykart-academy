#include "naive_gap_follow.hpp"
#include "queue"
#include "common.hpp"

std::optional<ScanPoint>
gap_follow::find_gap_naive(const std::vector<ScanPoint> &scan, uint8_t min_gap_size, float min_dist) {
    std::priority_queue<Gap> gaps{};

    uint32_t current_gap_len = 0;
    std::optional<uint32_t> current_gap_start;

    // Find gaps
    for (uint32_t i = 0; i < scan.size(); ++i) {
        float dist = scan[i].dist(ScanPoint::zero());

        // If point is part of gap
        if (dist >= min_dist) {
            current_gap_len++;

            if (!current_gap_start.has_value()) {
                current_gap_start = i;
            }
        }
            // Gap has ended
        else {
            // Add to gaps if long enough
            if (current_gap_len >= min_gap_size) {
                gaps.emplace(*current_gap_start, i - 1);
            }

            current_gap_len = 0;
            current_gap_start = std::nullopt;
        }
    }

    // Handle gap on the far right of the scan
    if (current_gap_len >= min_gap_size) {
        gaps.emplace(*current_gap_start, scan.size() - 1);
    }

    if (!gaps.empty()) {
        // Set middle index of our biggest gap as the target point
        auto biggest_gap = gaps.top();
        uint32_t middle_idx = biggest_gap.middle_idx();

        return scan[middle_idx];
    } else {
        return std::nullopt;
    }
}
