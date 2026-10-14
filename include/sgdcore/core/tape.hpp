#pragma once

#include "sgdcore/core/node.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace sgdcore {

class Tape {
public:
    Tape() = default;
    Tape(const Tape&) = delete;
    Tape& operator=(const Tape&) = delete;
    Tape(Tape&&) = delete;
    Tape& operator=(Tape&&) = delete;

    // Parent IDs are tape-local, must already exist, and must match op arity.
    NodeId record(OpCode op, NodeId lhs, NodeId rhs, double value);

    // Reserve room for a known number of future nodes without changing the graph.
    void reserve_nodes(std::size_t additional);

    void backward(NodeId root);

    void clear_adjoints() noexcept;

    void reset() noexcept;

    Node node(NodeId id) const;

    std::size_t size() const noexcept { return nodes_.size(); }

private:
    template <typename>
    friend class Var;

    void validate_handle(NodeId id, std::uint64_t generation) const;
    void set_variable_value(NodeId id, std::uint64_t generation, double value);
    void propagate_adjoints(NodeId id, std::vector<std::uint8_t>& active);

    std::vector<Node> nodes_;
    std::uint64_t generation_ = 0;
};

} // namespace sgdcore
