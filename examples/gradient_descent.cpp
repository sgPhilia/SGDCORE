#include <sgdcore/sgdcore.hpp>

#include <iomanip>
#include <iostream>

int main() {
    // Minimize f(x) = (x - 3)^2 with ten plain gradient-descent steps.
    double x_value = -2.0;
    constexpr double learning_rate = 0.2;
    constexpr int steps = 10;

    std::cout << std::fixed << std::setprecision(6)
              << "Minimize f(x) = (x - 3)^2\n"
              << "initial x = " << x_value << '\n';

    for (int step = 0; step < steps; ++step) {
        double derivative = 0.0;
        double loss_value = 0.0;
        {
            // Rebuild the graph each step because recorded forward values are
            // cached and set_value() does not reevaluate dependent operations.
            sgdcore::Tape tape;
            auto x = sgdcore::variable(x_value, tape);
            auto target = sgdcore::constant(3.0, tape);
            auto residual = x - target;
            auto loss = residual * residual;

            sgdcore::backward(loss);
            derivative = sgdcore::gradient(x);
            loss_value = sgdcore::value(loss);
        }

        x_value -= learning_rate * derivative;
        std::cout << "step " << (step + 1) << ": x = " << x_value
                  << ", f(x before update) = " << loss_value << '\n';
    }

    std::cout << "final x = " << x_value << '\n';
}
