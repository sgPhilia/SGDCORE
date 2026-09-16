#pragma once

#include "node.hpp"
#include <cstddef>
#include <vector>

namespace sgdcore {

class Tape {
public:
    Tape() = default;

    NodeId record(OpCode op, NodeId lhs, NodeId rhs, double value);

    void backward(NodeId root);

    void clear_adjoints() noexcept;

    void reset() noexcept;

    const Node& node(NodeId id) const { return nodes_[id]; }
    Node& node(NodeId id) { return nodes_[id]; }

    std::size_t size() const noexcept { return nodes_.size(); }

private:
    std::vector<Node> nodes_;
};

} // namespace sgdcore