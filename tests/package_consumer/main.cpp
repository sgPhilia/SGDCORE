#include <sgdcore/sgdcore.hpp>

#include <cmath>

int main() {
    sgdcore::Tape tape;
    auto x = sgdcore::variable(2.0, tape);
    auto y = sgdcore::variable(3.0, tape);
    auto z = x * y + sgdcore::sin(x) + 2.0;
    sgdcore::backward(z);

    const bool value_is_correct =
        std::abs(sgdcore::value(z) - (8.0 + std::sin(2.0))) < 1e-12;
    const bool x_gradient_is_correct =
        std::abs(sgdcore::gradient(x) - (3.0 + std::cos(2.0))) < 1e-12;
    const bool y_gradient_is_correct = std::abs(sgdcore::gradient(y) - 2.0) < 1e-12;
    return value_is_correct && x_gradient_is_correct && y_gradient_is_correct ? 0 : 1;
}
