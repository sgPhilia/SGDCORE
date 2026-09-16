#pragma once

#include <algorithm>
#include <cmath>

namespace sgdcore {

inline bool close_enough(double a, double b,
                         double rel_tol = 1e-9,
                         double abs_tol = 1e-12) noexcept {
    return std::fabs(a - b) <= abs_tol + rel_tol * std::max(std::fabs(a), std::fabs(b));
}

} // namespace sgdcore