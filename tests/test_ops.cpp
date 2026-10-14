#include <sgdcore/sgdcore.hpp>

#include <gtest/gtest.h>
#include <cmath>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>

using namespace sgdcore;

template <typename L, typename R>
concept AddExpression = requires(L lhs, R rhs) { lhs + rhs; };

static_assert(AddExpression<Var<double>, double>);
static_assert(AddExpression<int, Var<float>>);
static_assert(!AddExpression<Var<double>, Var<float>>);
static_assert(!AddExpression<Var<double>, std::string_view>);

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

TEST(Arithmetic, ScalarLiteralsWorkInBothOperandOrders) {
    Tape tape;
    auto x = variable(2.0, tape);

    const auto add_right = x + 2.0;
    const auto add_left = 2.0 + x;
    const auto sub_right = x - 2.0;
    const auto sub_left = 2.0 - x;
    const auto mul_right = x * 2.0;
    const auto mul_left = 2.0 * x;
    const auto div_right = x / 2.0;
    const auto div_left = 2.0 / x;
    const auto power_right = pow(x, 3);
    const auto power_left = pow(3.0, x);

    EXPECT_DOUBLE_EQ(add_right.value(), 4.0);
    EXPECT_DOUBLE_EQ(add_left.value(), 4.0);
    EXPECT_DOUBLE_EQ(sub_right.value(), 0.0);
    EXPECT_DOUBLE_EQ(sub_left.value(), 0.0);
    EXPECT_DOUBLE_EQ(mul_right.value(), 4.0);
    EXPECT_DOUBLE_EQ(mul_left.value(), 4.0);
    EXPECT_DOUBLE_EQ(div_right.value(), 1.0);
    EXPECT_DOUBLE_EQ(div_left.value(), 1.0);
    EXPECT_DOUBLE_EQ(power_right.value(), 8.0);
    EXPECT_DOUBLE_EQ(power_left.value(), 9.0);

    backward(sub_left);
    EXPECT_DOUBLE_EQ(gradient(x), -1.0);
    backward(div_left);
    EXPECT_DOUBLE_EQ(gradient(x), -0.5);
    backward(power_right);
    EXPECT_DOUBLE_EQ(gradient(x), 12.0);
    backward(power_left);
    EXPECT_DOUBLE_EQ(gradient(x), 9.0 * std::log(3.0));
}

TEST(Arithmetic, ScalarLiteralsPreserveVarTypeAndConvertToDouble) {
    Tape tape;
    auto float_x = variable(2.0F, tape);
    auto float_result = float_x + 2;
    static_assert(std::is_same_v<decltype(float_result), Var<float>>);
    EXPECT_DOUBLE_EQ(float_result.value(), 4.0);
    backward(float_result);
    EXPECT_DOUBLE_EQ(gradient(float_x), 1.0);

    auto int_x = variable(2, tape);
    auto int_result = 2.5 * int_x;
    static_assert(std::is_same_v<decltype(int_result), Var<int>>);
    EXPECT_DOUBLE_EQ(int_result.value(), 5.0);
    backward(int_result);
    EXPECT_DOUBLE_EQ(gradient(int_x), 2.5);
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

TEST(Operations, CrossTapeOperandsAreRejected) {
    Tape left_tape;
    Tape right_tape;
    auto left = variable(1.0, left_tape);
    auto right = variable(2.0, right_tape);
    EXPECT_THROW((left + right), std::invalid_argument);
    EXPECT_THROW(pow(left, right), std::invalid_argument);
}

TEST(Operations, DomainAndBoundaryForwardValuesFollowFloatingPointMath) {
    Tape tape;
    auto zero = variable(0.0, tape);
    auto negative = variable(-1.0, tape);
    auto one = constant(1.0, tape);

    EXPECT_TRUE(std::isinf((one / zero).value()));
    EXPECT_TRUE(std::isinf(log(zero).value()));
    EXPECT_LT(log(zero).value(), 0.0);
    EXPECT_TRUE(std::isnan(log(negative).value()));
    EXPECT_DOUBLE_EQ(sqrt(zero).value(), 0.0);
    EXPECT_TRUE(std::isnan(sqrt(negative).value()));
    EXPECT_DOUBLE_EQ(pow(negative, constant(3.0, tape)).value(), -1.0);
    EXPECT_TRUE(std::isnan(pow(negative, variable(0.5, tape)).value()));
    EXPECT_DOUBLE_EQ(pow(zero, constant(2.0, tape)).value(), 0.0);
}

TEST(Operations, ValueMutationIsLimitedToIndependentVariableLeaves) {
    Tape tape;
    auto x = variable(2.0, tape);
    backward(x);
    EXPECT_DOUBLE_EQ(gradient(x), 1.0);
    x.set_value(3.0);
    EXPECT_DOUBLE_EQ(x.value(), 3.0);
    EXPECT_DOUBLE_EQ(gradient(x), 0.0);

    auto y = x * x;
    EXPECT_THROW(x.set_value(4.0), std::logic_error);
    EXPECT_THROW(y.set_value(4.0), std::logic_error);
    EXPECT_DOUBLE_EQ(x.value(), 3.0);
    EXPECT_DOUBLE_EQ(y.value(), 9.0);
    backward(y);
    EXPECT_DOUBLE_EQ(gradient(x), 6.0);
}
