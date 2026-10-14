#pragma once

#include <cmath>
#include <stdexcept>

namespace sgdcore {

// h must be positive and finite; its useful magnitude depends on f and x.
template <typename F>
double central_difference(F&& f, double x, double h = 1e-5) {
    if (!std::isfinite(h) || h <= 0.0) {
        throw std::invalid_argument("central_difference step must be positive and finite");
    }
    return (f(x + h) - f(x - h)) / (2.0 * h);
}

} // namespace sgdcore
