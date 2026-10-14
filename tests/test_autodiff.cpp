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

TEST(Autodiff, DerivativesForEveryElementaryOperation) {
    const double x_value = 0.4;
    {
        Tape tape;
        auto x = variable(x_value, tape);
        auto y = cos(x);
        backward(y);
        EXPECT_DOUBLE_EQ(gradient(x), -std::sin(x_value));
    }
    {
        Tape tape;
        auto x = variable(x_value, tape);
        auto y = tan(x);
        backward(y);
        const double expected = 1.0 + std::tan(x_value) * std::tan(x_value);
        EXPECT_DOUBLE_EQ(gradient(x), expected);
    }
    {
        Tape tape;
        auto x = variable(2.0, tape);
        auto y = log(x);
        backward(y);
        EXPECT_DOUBLE_EQ(gradient(x), 0.5);
    }
    {
        Tape tape;
        auto x = variable(4.0, tape);
        auto y = sqrt(x);
        backward(y);
        EXPECT_DOUBLE_EQ(gradient(x), 0.25);
    }
    {
        Tape tape;
        auto x = variable(3.0, tape);
        auto y = -x;
        backward(y);
        EXPECT_DOUBLE_EQ(gradient(x), -1.0);
    }
    {
        Tape tape;
        auto x = variable(2.0, tape);
        auto y = x + constant(1.0, tape);
        backward(y);
        EXPECT_DOUBLE_EQ(gradient(x), 1.0);
    }
    {
        Tape tape;
        auto x = variable(2.0, tape);
        auto y = x - constant(1.0, tape);
        backward(y);
        EXPECT_DOUBLE_EQ(gradient(x), 1.0);
    }
}

TEST(Autodiff, RepeatedBackwardCallsReplacePreviousAdjoints) {
    Tape tape;
    auto x = variable(3.0, tape);
    auto y = x * x;

    backward(y);
    EXPECT_DOUBLE_EQ(gradient(x), 6.0);
    backward(y);
    EXPECT_DOUBLE_EQ(gradient(x), 6.0);
}

TEST(Autodiff, DomainBoundaryDerivativesFollowFloatingPointFormulas) {
    {
        Tape tape;
        auto numerator = variable(1.0, tape);
        auto denominator = variable(0.0, tape);
        auto quotient = numerator / denominator;
        EXPECT_TRUE(std::isinf(quotient.value()));
        backward(quotient);
        EXPECT_TRUE(std::isinf(gradient(numerator)));
        EXPECT_TRUE(std::isinf(gradient(denominator)));
    }
    {
        Tape tape;
        auto x = variable(0.0, tape);
        auto y = log(x);
        EXPECT_TRUE(std::isinf(y.value()));
        EXPECT_LT(y.value(), 0.0);
        backward(y);
        EXPECT_TRUE(std::isinf(gradient(x)));
        EXPECT_GT(gradient(x), 0.0);
    }
    {
        Tape tape;
        auto x = variable(-0.0, tape);
        auto y = log(x);
        EXPECT_TRUE(std::isinf(y.value()));
        backward(y);
        EXPECT_TRUE(std::isinf(gradient(x)));
        EXPECT_GT(gradient(x), 0.0);
    }
    {
        Tape tape;
        auto x = variable(-1.0, tape);
        auto y = log(x);
        EXPECT_TRUE(std::isnan(y.value()));
        backward(y);
        EXPECT_TRUE(std::isnan(gradient(x)));
    }
    {
        Tape tape;
        auto x = variable(-1.0, tape);
        auto invalid = log(x);
        auto root = invalid * constant(0.0, tape);
        backward(root);
        EXPECT_TRUE(std::isnan(gradient(x)));
    }
    {
        Tape tape;
        auto x = variable(-2.0, tape);
        auto unused_invalid = log(x);
        auto valid_root = x * x;
        backward(valid_root);
        EXPECT_TRUE(std::isnan(unused_invalid.value()));
        EXPECT_DOUBLE_EQ(gradient(x), -4.0);
    }
    {
        Tape tape;
        auto x = variable(0.0, tape);
        auto y = sqrt(x);
        backward(y);
        EXPECT_TRUE(std::isinf(gradient(x)));
    }
    {
        Tape tape;
        auto x = variable(-1.0, tape);
        auto y = sqrt(x);
        EXPECT_TRUE(std::isnan(y.value()));
        backward(y);
        EXPECT_TRUE(std::isnan(gradient(x)));
    }
    {
        Tape tape;
        auto base = variable(-2.0, tape);
        auto exponent = variable(2.0, tape);
        auto y = pow(base, exponent);
        EXPECT_DOUBLE_EQ(y.value(), 4.0);
        backward(y);
        EXPECT_DOUBLE_EQ(gradient(base), -4.0);
        EXPECT_TRUE(std::isnan(gradient(exponent)));
    }
    {
        Tape tape;
        auto base = variable(0.0, tape);
        auto exponent = variable(2.0, tape);
        auto y = pow(base, exponent);
        EXPECT_DOUBLE_EQ(y.value(), 0.0);
        backward(y);
        EXPECT_DOUBLE_EQ(gradient(base), 0.0);
        EXPECT_TRUE(std::isnan(gradient(exponent)));
    }
}

TEST(Autodiff, DeterministicArithmeticIdentitiesAcrossFiniteInputs) {
    for (int index = 1; index <= 20; ++index) {
        const double x_value = -2.0 + 0.2 * index;
        const double y_value = 0.5 + 0.15 * index;
        Tape tape;
        auto x = variable(x_value, tape);
        auto y = variable(y_value, tape);
        auto output = (x * y + x) / y;

        EXPECT_TRUE(std::isfinite(output.value()));
        EXPECT_NEAR(output.value(), x_value + x_value / y_value, 1e-12);
        backward(output);
        EXPECT_NEAR(gradient(x), 1.0 + 1.0 / y_value, 1e-12);
        EXPECT_NEAR(gradient(y), -x_value / (y_value * y_value), 1e-12);
    }
}

TEST(Autodiff, LongChainAndSharedBranchesRemainCorrect) {
    Tape tape;
    auto x = variable(0.25, tape);
    auto value = x;
    for (int i = 0; i < 500; ++i) {
        value = value * 1.0001 + 0.25;
    }
    const double expected_value =
        0.25 * std::pow(1.0001, 500) +
        0.25 * (std::pow(1.0001, 500) - 1.0) / (1.0001 - 1.0);
    EXPECT_NEAR(value.value(), expected_value, 1e-9);

    auto shared = sin(x) + x * x;
    auto left = shared + 3.0;
    auto right = 2.0 * shared;
    auto root = left + right;
    backward(root);
    EXPECT_NEAR(gradient(x), 3.0 * (std::cos(0.25) + 0.5), 1e-12);
}
