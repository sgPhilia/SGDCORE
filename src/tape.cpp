#include "sgdcore/core/tape.hpp"
#include "sgdcore/operations/traits.hpp"

#include <cmath>
#include <stdexcept>

namespace sgdcore {

namespace {

inline double operand_value(const Tape& tape, NodeId id) noexcept {
    return id == invalid_node_id ? 0.0 : tape.node(id).value;
}

void propagate_adjoints(const Node& node, Tape& tape) {
    const double a = operand_value(tape, node.lhs);
    const double b = operand_value(tape, node.rhs);

    double d_lhs = 0.0;
    double d_rhs = 0.0;

    switch (node.op) {
    case OpCode::Add:
        d_lhs = OperationTraits<OpCode::Add>::pdiff_lhs(a, b, node.value);
        d_rhs = OperationTraits<OpCode::Add>::pdiff_rhs(a, b, node.value);
        break;
    case OpCode::Sub:
        d_lhs = OperationTraits<OpCode::Sub>::pdiff_lhs(a, b, node.value);
        d_rhs = OperationTraits<OpCode::Sub>::pdiff_rhs(a, b, node.value);
        break;
    case OpCode::Mul:
        d_lhs = OperationTraits<OpCode::Mul>::pdiff_lhs(a, b, node.value);
        d_rhs = OperationTraits<OpCode::Mul>::pdiff_rhs(a, b, node.value);
        break;
    case OpCode::Div:
        d_lhs = OperationTraits<OpCode::Div>::pdiff_lhs(a, b, node.value);
        d_rhs = OperationTraits<OpCode::Div>::pdiff_rhs(a, b, node.value);
        break;
    case OpCode::Neg:
        d_lhs = OperationTraits<OpCode::Neg>::pdiff_lhs(a, b, node.value);
        break;
    case OpCode::Sin:
        d_lhs = OperationTraits<OpCode::Sin>::pdiff_lhs(a, b, node.value);
        break;
    case OpCode::Cos:
        d_lhs = OperationTraits<OpCode::Cos>::pdiff_lhs(a, b, node.value);
        break;
    case OpCode::Tan:
        d_lhs = OperationTraits<OpCode::Tan>::pdiff_lhs(a, b, node.value);
        break;
    case OpCode::Exp:
        d_lhs = OperationTraits<OpCode::Exp>::pdiff_lhs(a, b, node.value);
        break;
    case OpCode::Log:
        d_lhs = OperationTraits<OpCode::Log>::pdiff_lhs(a, b, node.value);
        break;
    case OpCode::Sqrt:
        d_lhs = OperationTraits<OpCode::Sqrt>::pdiff_lhs(a, b, node.value);
        break;
    case OpCode::Pow:
        d_lhs = OperationTraits<OpCode::Pow>::pdiff_lhs(a, b, node.value);
        d_rhs = OperationTraits<OpCode::Pow>::pdiff_rhs(a, b, node.value);
        break;
    case OpCode::Variable:
    case OpCode::Constant:
    default:
        return;
    }

    if (node.lhs != invalid_node_id) {
        tape.node(node.lhs).adjoint += node.adjoint * d_lhs;
    }
    if (node.rhs != invalid_node_id) {
        tape.node(node.rhs).adjoint += node.adjoint * d_rhs;
    }
}

} // namespace

NodeId Tape::record(OpCode op, NodeId lhs, NodeId rhs, double value) {
    if (nodes_.size() >= static_cast<std::size_t>(invalid_node_id)) {
        throw std::overflow_error("Tape node capacity exceeded");
    }
    const NodeId id = static_cast<NodeId>(nodes_.size());
    nodes_.push_back(Node{op, lhs, rhs, value, 0.0});
    return id;
}

void Tape::backward(NodeId root) {
    clear_adjoints();
    if (root >= nodes_.size()) {
        return;
    }

    nodes_[root].adjoint = 1.0;

    for (NodeId i = root; i != invalid_node_id; --i) {
        const Node& node = nodes_[i];
        if (node.adjoint != 0.0) {
            propagate_adjoints(node, *this);
        }
        if (i == 0) {
            break;
        }
    }
}

void Tape::clear_adjoints() noexcept {
    for (auto& n : nodes_) {
        n.adjoint = 0.0;
    }
}

void Tape::reset() noexcept {
    nodes_.clear();
}

} // namespace sgdcore