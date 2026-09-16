#pragma once

#include <cstdint>
#include <limits>

namespace sgdcore {

using NodeId = std::uint32_t;

inline constexpr NodeId invalid_node_id = std::numeric_limits<NodeId>::max();

} // namespace sgdcore