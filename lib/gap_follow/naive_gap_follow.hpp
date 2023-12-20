#pragma once

#include "ld06.hpp"

namespace gap_follow {
    /// Finds a target point to drive to by finding the largest gap in the scan.
    ///
    /// \param scan Lidar scan
    /// \param min_gap_size Minimum number of points in a gap required for it to be considered a gap
    /// \param min_dist Minimum distance for a point to be considered part of a gap, in m
    /// \return Target point to drive to, if a gap is found
    std::optional<ScanPoint>
    find_gap_naive(const std::vector<ScanPoint> &scan, uint8_t min_gap_size, float min_dist);
}