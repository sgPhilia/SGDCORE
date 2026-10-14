#pragma once

#include "sgdcore/core/var.hpp"
#include "sgdcore/operations/detail.hpp"
#include "sgdcore/operations/traits.hpp"

#include <type_traits>

namespace sgdcore {

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

template <typename T, typename U>
requires std::is_arithmetic_v<U>
Var<T> pow(const Var<T>& base, U exponent) {
    return pow(base, detail::scalar_constant_like(base, exponent));
}

template <typename U, typename T>
requires std::is_arithmetic_v<U>
Var<T> pow(U base, const Var<T>& exponent) {
    return pow(detail::scalar_constant_like(exponent, base), exponent);
}

} // namespace sgdcore
