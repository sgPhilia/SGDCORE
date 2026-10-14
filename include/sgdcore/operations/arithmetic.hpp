#pragma once

#include "sgdcore/core/var.hpp"
#include "sgdcore/operations/detail.hpp"
#include "sgdcore/operations/traits.hpp"

#include <type_traits>

namespace sgdcore {

template <typename T>
Var<T> operator+(const Var<T>& a, const Var<T>& b) {
    detail::check_same_tape(a, b);
    const double v = OperationTraits<OpCode::Add>::forward(a.value(), b.value());
    const NodeId id = a.tape()->record(OpCode::Add, a.node(), b.node(), v);
    return Var<T>(*a.tape(), id);
}

template <typename T>
Var<T> operator-(const Var<T>& a, const Var<T>& b) {
    detail::check_same_tape(a, b);
    const double v = OperationTraits<OpCode::Sub>::forward(a.value(), b.value());
    const NodeId id = a.tape()->record(OpCode::Sub, a.node(), b.node(), v);
    return Var<T>(*a.tape(), id);
}

template <typename T>
Var<T> operator*(const Var<T>& a, const Var<T>& b) {
    detail::check_same_tape(a, b);
    const double v = OperationTraits<OpCode::Mul>::forward(a.value(), b.value());
    const NodeId id = a.tape()->record(OpCode::Mul, a.node(), b.node(), v);
    return Var<T>(*a.tape(), id);
}

template <typename T>
Var<T> operator/(const Var<T>& a, const Var<T>& b) {
    detail::check_same_tape(a, b);
    const double v = OperationTraits<OpCode::Div>::forward(a.value(), b.value());
    const NodeId id = a.tape()->record(OpCode::Div, a.node(), b.node(), v);
    return Var<T>(*a.tape(), id);
}

template <typename T, typename U>
requires std::is_arithmetic_v<U>
Var<T> operator+(const Var<T>& a, U b) {
    return a + detail::scalar_constant_like(a, b);
}

template <typename U, typename T>
requires std::is_arithmetic_v<U>
Var<T> operator+(U a, const Var<T>& b) {
    return detail::scalar_constant_like(b, a) + b;
}

template <typename T, typename U>
requires std::is_arithmetic_v<U>
Var<T> operator-(const Var<T>& a, U b) {
    return a - detail::scalar_constant_like(a, b);
}

template <typename U, typename T>
requires std::is_arithmetic_v<U>
Var<T> operator-(U a, const Var<T>& b) {
    return detail::scalar_constant_like(b, a) - b;
}

template <typename T, typename U>
requires std::is_arithmetic_v<U>
Var<T> operator*(const Var<T>& a, U b) {
    return a * detail::scalar_constant_like(a, b);
}

template <typename U, typename T>
requires std::is_arithmetic_v<U>
Var<T> operator*(U a, const Var<T>& b) {
    return detail::scalar_constant_like(b, a) * b;
}

template <typename T, typename U>
requires std::is_arithmetic_v<U>
Var<T> operator/(const Var<T>& a, U b) {
    return a / detail::scalar_constant_like(a, b);
}

template <typename U, typename T>
requires std::is_arithmetic_v<U>
Var<T> operator/(U a, const Var<T>& b) {
    return detail::scalar_constant_like(b, a) / b;
}

template <typename T>
Var<T> operator-(const Var<T>& a) {
    const double v = OperationTraits<OpCode::Neg>::forward(a.value(), 0.0);
    const NodeId id = a.tape()->record(OpCode::Neg, a.node(), invalid_node_id, v);
    return Var<T>(*a.tape(), id);
}

} // namespace sgdcore
