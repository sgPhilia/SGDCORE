#include <sgdcore/sgdcore.hpp>

#include <cmath>
#include <iomanip>
#include <iostream>

namespace {

double scalar_function(double x) {
    return std::sin(x) * std::exp(x);
}

double autodiff_derivative(double x) {
    sgdcore::Tape tape;
    auto input = sgdcore::variable(x, tape);
    auto output = sgdcore::sin(input) * sgdcore::exp(input);
    sgdcore::backward(output);
    return sgdcore::gradient(input);
}

} // namespace

int main() {
    constexpr double x = 0.5;
    const double autodiff = autodiff_derivative(x);
    const double finite_difference =
        sgdcore::central_difference(scalar_function, x, 1e-5);

    std::cout << std::setprecision(12)
              << "x                  = " << x << '\n'
              << "autodiff derivative = " << autodiff << '\n'
              << "finite difference  = " << finite_difference << '\n'
              << "absolute difference = " << std::abs(autodiff - finite_difference) << '\n';
}
