#pragma once

#include "nodeid.hpp"
#include "tape.hpp"

#include <cstdint>
#include <stdexcept>
#include <type_traits>

namespace sgdcore {

template <typename T = double>
class Var {
    static_assert(std::is_arithmetic_v<T>, "Var<T> requires an arithmetic scalar type");

public:
    using value_type = T;

    Var() noexcept = default;

    explicit Var(Tape& tape, NodeId node)
        : tape_(&tape), node_(node), generation_(tape.generation_) {
        tape.validate_handle(node_, generation_);
    }

    Tape* tape() const noexcept { return tape_; }
    NodeId node() const noexcept { return node_; }

    bool is_valid() const noexcept {
        return tape_ != nullptr && node_ != invalid_node_id &&
               generation_ == tape_->generation_ && node_ < tape_->size();
    }

    double value() const {
        validate();
        return tape_->node(node_).value;
    }
    double adjoint() const {
        validate();
        return tape_->node(node_).adjoint;
    }

    void set_value(T v) {
        validate();
        tape_->set_variable_value(node_, generation_, static_cast<double>(v));
    }

private:
    void validate() const {
        if (tape_ == nullptr) {
            throw std::logic_error("Variable is not associated with a tape");
        }
        tape_->validate_handle(node_, generation_);
    }

    Tape* tape_ = nullptr;
    NodeId node_ = invalid_node_id;
    std::uint64_t generation_ = 0;
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
