#include "f1tenth_gap_follow.hpp"
#include "common.hpp"
#include "queue"
#include "algorithm"

std::optional<ScanPoint>
gap_follow::find_gap_bubble(std::vector<ScanPoint> scan, float bubble_radius, bool use_farthest) {
    // Find the closest point
    auto closest_pt = *std::min_element(scan.begin(), scan.end(),
                                        [](const ScanPoint &lhs, const ScanPoint &rhs) {
                                            return lhs.dist(ScanPoint::zero()) < rhs.dist(ScanPoint::zero());
                                        });

    // Find all points in bubble around closest point
    std::vector<uint32_t> zero_idxs{};
    for (uint32_t i = 0; i < scan.size(); ++i) {
        if (closest_pt.dist(scan[i]) < bubble_radius) {
            zero_idxs.push_back(i);
        }
    }
    // Zero points in the bubble
    for (uint32_t idx: zero_idxs) {
        scan[idx] = ScanPoint::zero();
    }

    std::priority_queue<Gap> gaps{};
    uint32_t current_gap_len = 0;
    std::optional<uint32_t> current_gap_start;

    // Find gaps (consecutive non zero points)
    for (uint32_t i = 0; i < scan.size(); ++i) {
        bool zeroed = scan[i].x == 0 && scan[i].y == 0;

        // If point is part of gap
        if (!zeroed) {
            current_gap_len++;

            if (!current_gap_start.has_value()) {
                current_gap_start = i;
            }
        }
            // Gap has ended
        else {
            if (current_gap_len > 0) {
                gaps.emplace(*current_gap_start, i - 1);
            }

            current_gap_len = 0;
            current_gap_start = std::nullopt;
        }
    }

    // Handle gap on the far right of the scan
    if (current_gap_len > 0) {
        gaps.emplace(*current_gap_start, scan.size() - 1);
    }

    if (!gaps.empty()) {
        auto biggest_gap = gaps.top();

        // Choose the farthest point as the target
        if (use_farthest) {
            auto farthest_point = *std::max_element(scan.begin() + (int) biggest_gap.start_idx,
                                                    scan.begin() + (int) biggest_gap.end_idx,
                                                    [](const ScanPoint &lhs, const ScanPoint &rhs) {
                                                        return lhs.dist(ScanPoint::zero()) < rhs.dist(ScanPoint::zero());
                                                    });

            return farthest_point;
        } else {
            // Else use center of gap
            return scan[biggest_gap.middle_idx()];
        }

    } else {
        return std::nullopt;
    }
}
