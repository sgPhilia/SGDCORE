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

#define SGDCORE_UNARY_FUNCTION(NAME, OPCODE)                          \
    template <typename T>                                             \
    Var<T> NAME(const Var<T>& a) {                                    \
        const double v = OperationTraits<OpCode::OPCODE>::forward(a.value(), 0.0); \
        const NodeId id = a.tape()->record(OpCode::OPCODE, a.node(), invalid_node_id, v); \
        return Var<T>(*a.tape(), id);                                 \
    }

SGDCORE_UNARY_FUNCTION(sin, Sin)
SGDCORE_UNARY_FUNCTION(cos, Cos)
SGDCORE_UNARY_FUNCTION(tan, Tan)
SGDCORE_UNARY_FUNCTION(exp, Exp)
SGDCORE_UNARY_FUNCTION(log, Log)
SGDCORE_UNARY_FUNCTION(sqrt, Sqrt)

#undef SGDCORE_UNARY_FUNCTION

template <typename T>
Var<T> pow(const Var<T>& base, const Var<T>& exponent) {
    detail::check_same_tape(base, exponent);
    const double v = OperationTraits<OpCode::Pow>::forward(base.value(), exponent.value());
    const NodeId id = base.tape()->record(
        OpCode::Pow, base.node(), exponent.node(), v);
    return Var<T>(*base.tape(), id);
}

} // namespace sgdcore