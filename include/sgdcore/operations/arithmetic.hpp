#pragma once

#include "sgdcore/core/var.hpp"
#include "sgdcore/operations/traits.hpp"

#include <stdexcept>

namespace sgdcore {

namespace detail {

template <typename T>
void check_same_tape(const Var<T>& a, const Var<T>& b) {
    if (a.tape() != b.tape()) {
        throw std::invalid_argument("Operands belong to different tapes");
    }
}

} // namespace detail

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

template <typename T>
Var<T> operator-(const Var<T>& a) {
    const double v = OperationTraits<OpCode::Neg>::forward(a.value(), 0.0);
    const NodeId id = a.tape()->record(OpCode::Neg, a.node(), invalid_node_id, v);
    return Var<T>(*a.tape(), id);
}

} // namespace sgdcore