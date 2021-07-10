#include "chip8.h"
#include "gtest/gtest.h"

TEST(OpCodeTests, TestOpCode0x01) {
    CHIP8 chip8 = CHIP8();

    uint16_t opcode = 0x1123;
    bool draw = chip8.exec_op(opcode);

    EXPECT_EQ(draw, false);

    uint16_t expected_PC = opcode & 0x0FFF;
    EXPECT_EQ(chip8.get_pc(), expected_PC);
}