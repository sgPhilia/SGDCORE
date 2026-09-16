#include <sgdcore/sgdcore.hpp>

#include <gtest/gtest.h>
#include <limits>

using namespace sgdcore;

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
}

TEST(Core, TapeReset) {
    Tape tape;
    tape.record(OpCode::Constant, invalid_node_id, invalid_node_id, 1.0);
    tape.reset();
    EXPECT_EQ(tape.size(), 0u);
}

TEST(Core, TapeClearAdjoints) {
    Tape tape;
    const NodeId n = tape.record(OpCode::Variable, invalid_node_id, invalid_node_id, 3.0);
    tape.node(n).adjoint = 5.0;
    tape.clear_adjoints();
    EXPECT_DOUBLE_EQ(tape.node(n).adjoint, 0.0);
}