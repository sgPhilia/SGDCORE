#include <sgdcore/sgdcore.hpp>
#include <iostream>

int main() {
    sgdcore::Tape tape;

    auto x = sgdcore::variable(2.0, tape);
    auto y = sgdcore::variable(3.0, tape);

    auto z = x * y + sgdcore::sin(x) + 2.0;

    sgdcore::backward(z);

    std::cout << "z = " << sgdcore::value(z) << "\n";
    std::cout << "dz/dx = " << sgdcore::gradient(x) << "\n";
    std::cout << "dz/dy = " << sgdcore::gradient(y) << "\n";

    return 0;
}
