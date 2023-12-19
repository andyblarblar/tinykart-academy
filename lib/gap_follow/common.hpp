#pragma once

#include "cstdint"

struct Gap {
    uint32_t start_idx;
    uint32_t end_idx;

    Gap(uint32_t startIdx, uint32_t endIdx) : start_idx(startIdx), end_idx(endIdx) {}

    [[nodiscard]] uint32_t middle_idx() const {
        return (end_idx + start_idx) / 2;
    }

    [[nodiscard]] uint32_t size() const {
        return end_idx - start_idx;
    }

    bool operator<(const Gap &rhs) const {
        return size() < rhs.size();
    }

    bool operator>(const Gap &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const Gap &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const Gap &rhs) const {
        return !(*this < rhs);
    }
};
