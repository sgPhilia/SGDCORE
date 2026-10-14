#include <sgdcore/sgdcore.hpp>

#include <gtest/gtest.h>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <type_traits>

using namespace sgdcore;

static_assert(!std::is_copy_constructible_v<Tape>);
static_assert(!std::is_move_constructible_v<Tape>);

TEST(Core, NodeIdSentinel) {
    EXPECT_EQ(invalid_node_id, std::numeric_limits<NodeId>::max());
}

TEST(Core, NodeSizeIsReasonable) {
    EXPECT_LE(sizeof(Node), 32u);
}

TEST(Core, TapeRecordsNodes) {
    Tape tape;
    const NodeId a = tape.record(OpCode::Constant, invalid_node_id, invalid_node_id, 1.0);
    const NodeId b = tape.record(OpCode::Constant, invalid_node_id, invalid_node_id, 2.0);
    EXPECT_EQ(a, 0u);
    EXPECT_EQ(b, 1u);
    EXPECT_EQ(tape.size(), 2u);
    EXPECT_DOUBLE_EQ(tape.node(a).value, 1.0);
    EXPECT_DOUBLE_EQ(tape.node(b).value, 2.0);
    auto snapshot = tape.node(a);
    snapshot.value = 8.0;
    EXPECT_DOUBLE_EQ(tape.node(a).value, 1.0);
}

TEST(Core, TapeReset) {
    Tape tape;
    tape.record(OpCode::Constant, invalid_node_id, invalid_node_id, 1.0);
    tape.reset();
    EXPECT_EQ(tape.size(), 0u);
}

TEST(Core, TapeClearAdjoints) {
    Tape tape;
    auto n = variable(3.0, tape);
    tape.backward(n.node());
    EXPECT_DOUBLE_EQ(n.adjoint(), 1.0);
    tape.clear_adjoints();
    EXPECT_DOUBLE_EQ(n.adjoint(), 0.0);
}

TEST(Core, CheckedNodeAndParentIds) {
    Tape tape;
    const auto initial_size = tape.size();
    EXPECT_THROW(tape.node(0), std::out_of_range);
    EXPECT_THROW(tape.backward(0), std::out_of_range);
    EXPECT_THROW(tape.record(OpCode::Neg, 0, invalid_node_id, 0.0), std::out_of_range);
    EXPECT_THROW(tape.record(OpCode::Add, invalid_node_id, invalid_node_id, 0.0),
                 std::invalid_argument);
    EXPECT_THROW(tape.record(static_cast<OpCode>(255), invalid_node_id,
                             invalid_node_id, 0.0),
                 std::invalid_argument);
    EXPECT_THROW(tape.reserve_nodes(std::numeric_limits<std::size_t>::max()),
                 std::overflow_error);
    EXPECT_EQ(tape.size(), initial_size);

    const NodeId first = tape.record(OpCode::Variable, invalid_node_id, invalid_node_id, 1.0);
    EXPECT_THROW(tape.record(OpCode::Neg, first + 1, invalid_node_id, 0.0),
                 std::out_of_range);
}

TEST(Core, ResetInvalidatesExistingHandles) {
    Tape tape;
    auto old = variable(1.0, tape);
    tape.reset();
    auto replacement = variable(9.0, tape);
    EXPECT_EQ(old.node(), replacement.node());
    EXPECT_FALSE(old.is_valid());
    EXPECT_THROW(old.value(), std::logic_error);
    EXPECT_THROW(gradient(old), std::logic_error);
    EXPECT_THROW(backward(old), std::logic_error);
    EXPECT_THROW((old + replacement), std::logic_error);
    const auto size_before_invalid_literal = tape.size();
    EXPECT_THROW((old + 2.0), std::logic_error);
    EXPECT_EQ(tape.size(), size_before_invalid_literal);
    EXPECT_DOUBLE_EQ(replacement.value(), 9.0);
}

TEST(Core, DefaultAndOutOfRangeHandlesFailClearly) {
    Tape tape;
    Var<double> empty;
    EXPECT_FALSE(empty.is_valid());
    EXPECT_THROW(empty.value(), std::logic_error);
    EXPECT_THROW(gradient(empty), std::logic_error);
    EXPECT_THROW(backward(empty), std::logic_error);
    EXPECT_THROW((Var<double>(tape, 4)), std::out_of_range);
}

TEST(Core, RecordingEnforcesEarlierParentsAndOperationArity) {
    Tape tape;
    const NodeId x = tape.record(OpCode::Variable, invalid_node_id, invalid_node_id, 2.0);
    const NodeId y = tape.record(OpCode::Neg, x, invalid_node_id, -2.0);
    EXPECT_EQ(y, 1u);
    EXPECT_THROW(tape.record(OpCode::Sin, x, x, 0.0), std::invalid_argument);
    EXPECT_THROW(tape.record(OpCode::Add, x, invalid_node_id, 0.0), std::invalid_argument);
}
