#include <sgdcore/sgdcore.hpp>

#include <gtest/gtest.h>
#include <cmath>

using namespace sgdcore;

TEST(Autodiff, LinearFunction) {
    Tape tape;
    auto x = variable(2.0, tape);
    auto three = constant(3.0, tape);
    auto four = constant(4.0, tape);
    auto y = three * x + four;

    backward(y);
    EXPECT_DOUBLE_EQ(gradient(x), 3.0);
}

TEST(Autodiff, ProductRule) {
    Tape tape;
    auto x = variable(2.0, tape);
    auto y = variable(3.0, tape);
    auto z = x * y;

    backward(z);
    EXPECT_DOUBLE_EQ(gradient(x), 3.0);
    EXPECT_DOUBLE_EQ(gradient(y), 2.0);
}

TEST(Autodiff, MultipleUsage) {
    Tape tape;
    auto x = variable(2.0, tape);
    auto y = x * x + x; // f(x) = x^2 + x, f'(x) = 2x + 1 = 5

    backward(y);
    EXPECT_DOUBLE_EQ(gradient(x), 5.0);
}

TEST(Autodiff, SinGradient) {
    Tape tape;
    auto x = variable(0.5, tape);
    auto y = sin(x);

    backward(y);
    EXPECT_DOUBLE_EQ(gradient(x), std::cos(0.5));
}

TEST(Autodiff, ChainRule) {
    Tape tape;
    auto x = variable(0.5, tape);
    auto y = exp(sin(x));

    backward(y);
    const double expected = std::exp(std::sin(0.5)) * std::cos(0.5);
    EXPECT_DOUBLE_EQ(gradient(x), expected);
}

TEST(Autodiff, DivisionGradient) {
    Tape tape;
    auto x = variable(4.0, tape);
    auto y = variable(2.0, tape);
    auto z = x / y;

    backward(z);
    EXPECT_DOUBLE_EQ(gradient(x), 1.0 / 2.0);
    EXPECT_DOUBLE_EQ(gradient(y), -4.0 / (2.0 * 2.0));
}

TEST(Autodiff, PowerGradient) {
    Tape tape;
    auto x = variable(2.0, tape);
    auto y = variable(3.0, tape);
    auto z = pow(x, y);

    backward(z);
    const double d_x = 3.0 * std::pow(2.0, 2.0); // y * x^(y-1)
    const double d_y = std::pow(2.0, 3.0) * std::log(2.0);
    EXPECT_DOUBLE_EQ(gradient(x), d_x);
    EXPECT_DOUBLE_EQ(gradient(y), d_y);
}