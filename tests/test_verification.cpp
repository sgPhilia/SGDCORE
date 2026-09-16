#include <sgdcore/sgdcore.hpp>

#include <gtest/gtest.h>
#include <cmath>

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
        if (h >= 1e-6) {
            EXPECT_TRUE(close_enough(ad, fd, 1e-5, 1e-8))
                << "h = " << h;
        }
    }
}