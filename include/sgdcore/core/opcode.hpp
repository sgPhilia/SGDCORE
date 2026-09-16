#pragma once

#include <cstdint>

namespace sgdcore {

enum class OpCode : std::uint8_t {
    Variable,
    Constant,
    Add,
    Sub,
    Mul,
    Div,
    Neg,
    Sin,
    Cos,
    Tan,
    Exp,
    Log,
    Sqrt,
    Pow
};

} // namespace sgdcore