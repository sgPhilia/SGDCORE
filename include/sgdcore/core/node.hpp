#pragma once

#include "sgdcore/core/nodeid.hpp"
#include "sgdcore/core/opcode.hpp"

namespace sgdcore {

struct Node {
    double value;
    double adjoint;
    NodeId lhs;
    NodeId rhs;
    OpCode op;
};

} // namespace sgdcore
