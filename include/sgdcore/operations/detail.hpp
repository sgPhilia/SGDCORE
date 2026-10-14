#pragma once

#include "sgdcore/core/var.hpp"

#include <stdexcept>
#include <type_traits>

namespace sgdcore::detail {

template <typename T, typename U>
requires std::is_arithmetic_v<U>
Var<T> scalar_constant_like(const Var<T>& variable, U value) {
    // Validate the handle before recording a constant, so stale handles do not
    // leave unreachable nodes behind when an expression fails.
    (void)variable.value();
    // A literal expression records a constant and then an operation. Reserve
    // both slots first, so allocation failure cannot leave an orphan constant.
    variable.tape()->reserve_nodes(2);
    const NodeId id = variable.tape()->record(
        OpCode::Constant, invalid_node_id, invalid_node_id,
        static_cast<double>(value));
    return Var<T>(*variable.tape(), id);
}

template <typename T>
void check_same_tape(const Var<T>& a, const Var<T>& b) {
    (void)a.value();
    (void)b.value();
    if (a.tape() != b.tape()) {
        throw std::invalid_argument("Operands belong to different tapes");
    }
}

} // namespace sgdcore::detail
