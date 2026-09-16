#pragma once

#include "sgdcore/core/var.hpp"

#include <stdexcept>

namespace sgdcore::detail {

template <typename T>
void check_same_tape(const Var<T>& a, const Var<T>& b) {
    if (a.tape() != b.tape()) {
        throw std::invalid_argument("Operands belong to different tapes");
    }
}

} // namespace sgdcore::detail
