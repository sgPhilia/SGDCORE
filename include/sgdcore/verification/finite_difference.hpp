#pragma once

#include <utility>

namespace sgdcore {

template <typename F>
double central_difference(F&& f, double x, double h = 1e-5) {
    return (f(x + h) - f(x - h)) / (2.0 * h);
}

} // namespace sgdcore