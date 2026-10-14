#include <sgdcore/sgdcore.hpp>

#include <gtest/gtest.h>
#include <cmath>
#include <limits>
#include <stdexcept>

using namespace sgdcore;

double scalar_fn(double x) {
    return std::sin(x) * std::exp(x);
}

double ad_gradient(double x) {
    Tape tape;
    auto v = variable(x, tape);
    auto out = sin(v) * exp(v);
    backward(out);
    return gradient(v);
}

TEST(Verification, AgainstCentralDifference) {
    const double x = 0.5;
    const double ad = ad_gradient(x);
    const double fd = central_difference(scalar_fn, x, 1e-5);
    EXPECT_TRUE(close_enough(ad, fd, 1e-6, 1e-9));
}

TEST(Verification, MultipleSteps) {
    const double x = 1.2;
    auto f = [](double v) { return std::pow(v, 3.0) + std::log(v); };
    auto ad_f = [](double v) {
        Tape tape;
        auto xv = variable(v, tape);
        auto out = pow(xv, constant(3.0, tape)) + log(xv);
        backward(out);
        return gradient(xv);
    };

    for (double h = 1e-2; h >= 1e-8; h *= 0.1) {
        const double ad = ad_f(x);
        const double fd = central_difference(f, x, h);

        if (h >= 1e-3) {
            EXPECT_TRUE(close_enough(ad, fd, 1e-4, 1e-8))
                << "h = " << h;
        }
    }
}

TEST(Verification, CompositeExpressionsMatchCentralDifferences) {
    const auto compare = [](auto scalar_function, auto expression, double x_value) {
        Tape tape;
        auto x = variable(x_value, tape);
        auto output = expression(x);
        backward(output);
        const double numerical = central_difference(scalar_function, x_value, 1e-5);
        EXPECT_TRUE(std::isfinite(gradient(x)));
        EXPECT_TRUE(close_enough(gradient(x), numerical, 2e-6, 1e-8));
    };

    compare(
        [](double x) { return std::sin(x) * std::exp(x); },
        [](const auto& x) { return sin(x) * exp(x); }, 0.6);
    compare(
        [](double x) { return std::log(x) + x * std::sqrt(x); },
        [](const auto& x) { return log(x) + x * sqrt(x); }, 1.4);
    compare(
        [](double x) { return (x * x * x) / (1.0 + x); },
        [](const auto& x) { return pow(x, 3) / (1.0 + x); }, 1.2);
}

TEST(Verification, CentralDifferenceRejectsInvalidSteps) {
    auto f = [](double x) { return x * x; };
    EXPECT_THROW(central_difference(f, 1.0, 0.0), std::invalid_argument);
    EXPECT_THROW(central_difference(f, 1.0, -1e-5), std::invalid_argument);
    EXPECT_THROW(central_difference(f, 1.0, std::numeric_limits<double>::infinity()),
                 std::invalid_argument);
    EXPECT_THROW(central_difference(f, 1.0, std::numeric_limits<double>::quiet_NaN()),
                 std::invalid_argument);
}
