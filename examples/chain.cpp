#include <sgdcore/sgdcore.hpp>
#include <iostream>

int main() {
    sgdcore::Tape tape;

    auto x = sgdcore::variable(0.5, tape);
    auto y = x;

    for (int i = 0; i < 5; ++i) {
        y = sgdcore::exp(y) + y * y;
    }

    sgdcore::backward(y);

    std::cout << "y = " << sgdcore::value(y) << "\n";
    std::cout << "dy/dx = " << sgdcore::gradient(x) << "\n";

    return 0;
}