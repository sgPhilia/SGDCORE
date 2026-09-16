#pragma once

#include "sgdcore/core/node_id.hpp"
#include "sgdcore/core/opcode.hpp"
#include "sgdcore/core/node.hpp"
#include "sgdcore/core/tape.hpp"
#include "sgdcore/core/var.hpp"
#include "sgdcore/operations/traits.hpp"
#include "sgdcore/operations/arithmetic.hpp"
#include "sgdcore/operations/elementary.hpp"
#include "sgdcore/autodiff/reverse.hpp"
#include "sgdcore/verification/finite_difference.hpp"
#include "sgdcore/verification/tolerance.hpp"