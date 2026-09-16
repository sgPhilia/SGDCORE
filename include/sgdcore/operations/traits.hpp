#pragma once

#include "sgdcore/core/opcode.hpp"

#include <cmath>

namespace sgdcore {

template <OpCode Op>
struct OperationTraits;

template <>
struct OperationTraits<OpCode::Add> {
    static double forward(double a, double b) noexcept { return a + b; }
    static double pdiff_lhs(double /*a*/, double /*b*/, double /*result*/) noexcept { return 1.0; }
    static double pdiff_rhs(double /*a*/, double /*b*/, double /*result*/) noexcept { return 1.0; }
};

template <>
struct OperationTraits<OpCode::Sub> {
    static double forward(double a, double b) noexcept { return a - b; }
    static double pdiff_lhs(double /*a*/, double /*b*/, double /*result*/) noexcept { return 1.0; }
    static double pdiff_rhs(double /*a*/, double /*b*/, double /*result*/) noexcept { return -1.0; }
};

template <>
struct OperationTraits<OpCode::Mul> {
    static double forward(double a, double b) noexcept { return a * b; }
    static double pdiff_lhs(double /*a*/, double b, double /*result*/) noexcept { return b; }
    static double pdiff_rhs(double a, double /*b*/, double /*result*/) noexcept { return a; }
};

template <>
struct OperationTraits<OpCode::Div> {
    static double forward(double a, double b) noexcept { return a / b; }
    static double pdiff_lhs(double /*a*/, double b, double /*result*/) noexcept { return 1.0 / b; }
    static double pdiff_rhs(double a, double b, double /*result*/) noexcept { return -a / (b * b); }
};

template <>
struct OperationTraits<OpCode::Neg> {
    static double forward(double a, double /*b*/) noexcept { return -a; }
    static double pdiff_lhs(double /*a*/, double /*b*/, double /*result*/) noexcept { return -1.0; }
    static double pdiff_rhs(double /*a*/, double /*b*/, double /*result*/) noexcept { return 0.0; }
};

template <>
struct OperationTraits<OpCode::Sin> {
    static double forward(double a, double /*b*/) noexcept { return std::sin(a); }
    static double pdiff_lhs(double a, double /*b*/, double /*result*/) noexcept { return std::cos(a); }
    static double pdiff_rhs(double /*a*/, double /*b*/, double /*result*/) noexcept { return 0.0; }
};

template <>
struct OperationTraits<OpCode::Cos> {
    static double forward(double a, double /*b*/) noexcept { return std::cos(a); }
    static double pdiff_lhs(double a, double /*b*/, double /*result*/) noexcept { return -std::sin(a); }
    static double pdiff_rhs(double /*a*/, double /*b*/, double /*result*/) noexcept { return 0.0; }
};

template <>
struct OperationTraits<OpCode::Tan> {
    static double forward(double a, double /*b*/) noexcept { return std::tan(a); }
    static double pdiff_lhs(double /*a*/, double /*b*/, double result) noexcept { return 1.0 + result * result; }
    static double pdiff_rhs(double /*a*/, double /*b*/, double /*result*/) noexcept { return 0.0; }
};

template <>
struct OperationTraits<OpCode::Exp> {
    static double forward(double a, double /*b*/) noexcept { return std::exp(a); }
    static double pdiff_lhs(double /*a*/, double /*b*/, double result) noexcept { return result; }
    static double pdiff_rhs(double /*a*/, double /*b*/, double /*result*/) noexcept { return 0.0; }
};

template <>
struct OperationTraits<OpCode::Log> {
    static double forward(double a, double /*b*/) noexcept { return std::log(a); }
    static double pdiff_lhs(double a, double /*b*/, double /*result*/) noexcept { return 1.0 / a; }
    static double pdiff_rhs(double /*a*/, double /*b*/, double /*result*/) noexcept { return 0.0; }
};

template <>
struct OperationTraits<OpCode::Sqrt> {
    static double forward(double a, double /*b*/) noexcept { return std::sqrt(a); }
    static double pdiff_lhs(double /*a*/, double /*b*/, double result) noexcept { return 0.5 / result; }
    static double pdiff_rhs(double /*a*/, double /*b*/, double /*result*/) noexcept { return 0.0; }
};

template <>
struct OperationTraits<OpCode::Pow> {
    static double forward(double a, double b) noexcept { return std::pow(a, b); }
    static double pdiff_lhs(double a, double b, double /*result*/) noexcept {
        return b * std::pow(a, b - 1.0);
    }
    static double pdiff_rhs(double a, double b, double result) noexcept {
        return result * std::log(a);
    }
};

} // namespace sgdcore