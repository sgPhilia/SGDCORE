#pragma once

#include "sgdcore/core/tape.hpp"
#include "sgdcore/core/var.hpp"

namespace sgdcore {

template <typename T>
void backward(const Var<T>& root) {
    (void)root.value();
    root.tape()->backward(root.node());
}

template <typename T>
double gradient(const Var<T>& wrt) {
    return wrt.adjoint();
}

template <typename T>
double value(const Var<T>& v) {
    return v.value();
}

inline void clear_adjoints(Tape& tape) noexcept {
    tape.clear_adjoints();
}

inline void reset(Tape& tape) noexcept {
    tape.reset();
}

} // namespace sgdcore
