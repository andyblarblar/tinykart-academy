#pragma once

#include "ld06.hpp"

namespace gap_follow {
    /// Finds a target point to drive to by finding the largest open area in the scan.
    ///
    /// \param scan Lidar scan
    /// \param bubble_radius Radius in meters around the closest scan point to remove points in, creating a gap
    /// \param use_farthest Chooses the farthest point in the gap for target point if true. Else, uses midpoint
    /// \return Target point to drive to, if a gap is found
    std::optional<ScanPoint>
    find_gap_bubble(std::vector<ScanPoint> scan, float bubble_radius, bool use_farthest = true);
}