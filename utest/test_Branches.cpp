#include <gtest/gtest.h>

#include "../core/cp6502.hpp"

using namespace cp6502;

struct BranchesTests : public testing::Test {
    Mem mem;
    CPU cpu;

    virtual void SetUp() {
        cpu.Reset(0xFF00, mem);
    }

    virtual void TearDown() {
    }
};

TEST_F(BranchesTests, BEQBranchesWhenZIsSet) {
    // given:
    cpu.Z = true;
    mem[0xFF00] = CPU::INS_BEQ;
    mem[0xFF01] = 0x01;         // branch to 0xFF03
    constexpr s32 EXPECTED_CYCLES = 3;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC, 0xFF03);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
}

TEST_F(BranchesTests, BEQBranchesBackwordsWhenZIsSet) {
    // given:
    cpu.Reset(0xFFCC, mem);
    cpu.Z = true;
    signed char offset = -2;
    mem[0xFFCC] = CPU::INS_BEQ;
    mem[0xFFCD] = offset;  // branch to 0xFFCC
    constexpr s32 EXPECTED_CYCLES = 3;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC, 0xFFCC);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
}

TEST_F(BranchesTests, BEQDoesNotBranchWhenZIsNotSet) {
    // given:
    cpu.Z = false;
    mem[0xFF00] = CPU::INS_BEQ;
    mem[0xFF01] = 0x01;         // branch to 0xFF03
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC, 0xFF02);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
}

TEST_F(BranchesTests, BEQBranchesCrossPage) {
    // given:
    cpu.Reset(0xFEFD, mem);
    cpu.Z = true;
    mem[0xFEFD] = CPU::INS_BEQ;
    mem[0xFEFE] = 0x1;
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC, 0xFF00);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
}

TEST_F(BranchesTests, BEQBranchesBackwordsCrossPage) {
    // given:
    cpu.Reset(0xFF00, mem);
    cpu.Z = true;
    signed char offset = -3;
    mem[0xFF00] = CPU::INS_BEQ;
    mem[0xFF01] = offset;  // branch to 0xFEFF
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC, 0xFEFF);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
}

TEST_F(BranchesTests, BEQInProgram) {
    // given:
    cpu.Reset(0xFF00, mem);
    cpu.Z = true;
    /*
        loop
        lda #0
        beq loop
    */
    mem[0xFF00] = 0xA9;
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0xF0;
    mem[0xFF03] = 0xFC;
    constexpr s32 EXPECTED_CYCLES = 2 + 3;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC, 0xFF00);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
}
