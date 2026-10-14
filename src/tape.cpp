#include "sgdcore/core/tape.hpp"
#include "sgdcore/operations/traits.hpp"

#include <stdexcept>

namespace sgdcore {

namespace {

inline double operand_value(const Tape& tape, NodeId id) {
    return id == invalid_node_id ? 0.0 : tape.node(id).value;
}

} // namespace

void Tape::reserve_nodes(std::size_t additional) {
    const auto max_nodes = static_cast<std::size_t>(invalid_node_id);
    if (nodes_.size() > max_nodes || additional > max_nodes - nodes_.size()) {
        throw std::overflow_error("Tape node capacity exceeded");
    }
    nodes_.reserve(nodes_.size() + additional);
}

NodeId Tape::record(OpCode op, NodeId lhs, NodeId rhs, double value) {
    if (nodes_.size() >= static_cast<std::size_t>(invalid_node_id)) {
        throw std::overflow_error("Tape node capacity exceeded");
    }

    const auto valid_parent = [this](NodeId parent) {
        return parent == invalid_node_id || parent < nodes_.size();
    };
    if (!valid_parent(lhs) || !valid_parent(rhs)) {
        throw std::out_of_range("Tape parent node ID is out of range");
    }

    bool unary = false;
    bool binary = false;
    bool leaf = false;
    switch (op) {
    case OpCode::Variable:
    case OpCode::Constant: leaf = true; break;
    case OpCode::Neg:
    case OpCode::Sin:
    case OpCode::Cos:
    case OpCode::Tan:
    case OpCode::Exp:
    case OpCode::Log:
    case OpCode::Sqrt: unary = true; break;
    case OpCode::Add:
    case OpCode::Sub:
    case OpCode::Mul:
    case OpCode::Div:
    case OpCode::Pow: binary = true; break;
    default: throw std::invalid_argument("Unknown tape operation code");
    }
    if ((leaf && (lhs != invalid_node_id || rhs != invalid_node_id)) ||
        (unary && (lhs == invalid_node_id || rhs != invalid_node_id)) ||
        (binary && (lhs == invalid_node_id || rhs == invalid_node_id))) {
        throw std::invalid_argument("Tape parent IDs do not match operation arity");
    }

    const NodeId id = static_cast<NodeId>(nodes_.size());
    nodes_.push_back(Node{value, 0.0, lhs, rhs, op});
    return id;
}

Node Tape::node(NodeId id) const {
    if (id >= nodes_.size()) {
        throw std::out_of_range("Tape node ID is out of range");
    }
    return nodes_[id];
}

void Tape::validate_handle(NodeId id, std::uint64_t generation) const {
    if (generation != generation_) {
        throw std::logic_error("Variable handle is stale after tape reset");
    }
    if (id == invalid_node_id || id >= nodes_.size()) {
        throw std::out_of_range("Tape node ID is out of range");
    }
}

void Tape::set_variable_value(NodeId id, std::uint64_t generation, double value) {
    validate_handle(id, generation);
    if (nodes_[id].op != OpCode::Variable) {
        throw std::logic_error("Only variable leaf nodes can be mutated");
    }
    for (std::size_t i = static_cast<std::size_t>(id) + 1; i < nodes_.size(); ++i) {
        if (nodes_[i].lhs == id || nodes_[i].rhs == id) {
            throw std::logic_error("Cannot mutate a variable after recording a dependent operation");
        }
    }
    nodes_[id].value = value;
    clear_adjoints();
}

void Tape::propagate_adjoints(NodeId id, std::vector<std::uint8_t>& active) {
    const Node& node = nodes_[id];
    const double a = operand_value(*this, node.lhs);
    const double b = operand_value(*this, node.rhs);

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
        nodes_[node.lhs].adjoint += node.adjoint * d_lhs;
        active[node.lhs] = 1;
    }
    if (node.rhs != invalid_node_id) {
        nodes_[node.rhs].adjoint += node.adjoint * d_rhs;
        active[node.rhs] = 1;
    }
}

void Tape::backward(NodeId root) {
    if (root >= nodes_.size()) {
        throw std::out_of_range("Backward root node ID is out of range");
    }
    clear_adjoints();

    std::vector<std::uint8_t> active(nodes_.size(), 0);
    active[root] = 1;
    nodes_[root].adjoint = 1.0;

    for (NodeId i = root; i != invalid_node_id; --i) {
        if (active[i] != 0) {
            propagate_adjoints(i, active);
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
    ++generation_;
}

} // namespace sgdcore
