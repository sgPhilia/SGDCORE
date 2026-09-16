#pragma once

#include "node_id.hpp"
#include "tape.hpp"

#include <type_traits>

namespace sgdcore {

template <typename T = double>
class Var {
public:
    using value_type = T;

    Var() noexcept = default;

    explicit Var(Tape& tape, NodeId node) noexcept
        : tape_(&tape), node_(node) {}

    Tape* tape() const noexcept { return tape_; }
    NodeId node() const noexcept { return node_; }

    bool is_valid() const noexcept {
        return tape_ != nullptr && node_ != invalid_node_id;
    }

    double value() const { return tape_->node(node_).value; }
    double adjoint() const { return tape_->node(node_).adjoint; }

    void set_value(T v) { tape_->node(node_).value = static_cast<double>(v); }

private:
    Tape* tape_ = nullptr;
    NodeId node_ = invalid_node_id;
};

template <typename T>
Var<T> variable(T value, Tape& tape) {
    static_assert(std::is_arithmetic_v<T>, "variable value must be arithmetic");
    const NodeId id = tape.record(
        OpCode::Variable, invalid_node_id, invalid_node_id,
        static_cast<double>(value));
    return Var<T>(tape, id);
}

template <typename T>
Var<T> constant(T value, Tape& tape) {
    static_assert(std::is_arithmetic_v<T>, "constant value must be arithmetic");
    const NodeId id = tape.record(
        OpCode::Constant, invalid_node_id, invalid_node_id,
        static_cast<double>(value));
    return Var<T>(tape, id);
}

} // namespace sgdcore