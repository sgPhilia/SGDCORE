#include <sgdcore/sgdcore.hpp>

#include <gtest/gtest.h>
#include <cmath>

using namespace sgdcore;

TEST(Arithmetic, AddSubMulDiv) {
    Tape tape;
    auto x = variable(2.0, tape);
    auto y = variable(4.0, tape);
    auto s = x + y;
    auto d = x - y;
    auto p = x * y;
    auto q = x / y;

    EXPECT_DOUBLE_EQ(s.value(), 6.0);
    EXPECT_DOUBLE_EQ(d.value(), -2.0);
    EXPECT_DOUBLE_EQ(p.value(), 8.0);
    EXPECT_DOUBLE_EQ(q.value(), 0.5);
}

TEST(Arithmetic, UnaryNeg) {
    Tape tape;
    auto x = variable(3.0, tape);
    auto n = -x;
    EXPECT_DOUBLE_EQ(n.value(), -3.0);
}

TEST(Elementary, SinCosTan) {
    Tape tape;
    auto x = variable(0.5, tape);
    EXPECT_DOUBLE_EQ(sin(x).value(), std::sin(0.5));
    EXPECT_DOUBLE_EQ(cos(x).value(), std::cos(0.5));
    EXPECT_DOUBLE_EQ(tan(x).value(), std::tan(0.5));
}

TEST(Elementary, ExpLogSqrt) {
    Tape tape;
    auto x = variable(2.0, tape);
    EXPECT_DOUBLE_EQ(exp(x).value(), std::exp(2.0));
    EXPECT_DOUBLE_EQ(log(x).value(), std::log(2.0));
    EXPECT_DOUBLE_EQ(sqrt(x).value(), std::sqrt(2.0));
}

TEST(Elementary, Pow) {
    Tape tape;
    auto x = variable(2.0, tape);
    auto y = variable(3.0, tape);
    auto z = pow(x, y);
    EXPECT_DOUBLE_EQ(z.value(), std::pow(2.0, 3.0));
}