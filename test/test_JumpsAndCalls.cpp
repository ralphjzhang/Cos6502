#include <gtest/gtest.h>

#include "../core/cp6502.hpp"

using namespace cp6502;

struct JumpsAndCallsTests : public testing::Test {
    Mem mem;
    CPU cpu;

    virtual void SetUp() {
        cpu.Reset(0xFF00, mem);
    }

    virtual void TearDown() {
    }
};

TEST_F(JumpsAndCallsTests, CanJumpToASubroutineAndJumpBack) {
    // given:
    mem[0xFF00] = CPU::INS_JSR;
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    mem[0x8000] = CPU::INS_RTS;
    mem[0xFF03] = CPU::INS_LDA_IM;
    mem[0xFF04] = 0x42;
    constexpr s32 EXPECTED_CYCLES = 6 + 6 + 2;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_EQ(cpu.SP, cpuCopy.SP);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
}

TEST_F(JumpsAndCallsTests, JSRDoesNotAffectProcessorStatus) {
    // given:
    mem[0xFF00] = CPU::INS_JSR;
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC, 0x8000);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
    EXPECT_NE(cpu.SP, cpuCopy.SP);
}

TEST_F(JumpsAndCallsTests, RTSDoesNotAffectProcessorStatus) {
    // given:
    mem[0xFF00] = CPU::INS_JSR;
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    mem[0x8000] = CPU::INS_RTS;
    constexpr s32 EXPECTED_CYCLES = 6 + 6;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC, 0xFF03);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
}

TEST_F(JumpsAndCallsTests, JumpAbsoluteCanJumpToNewLocation) {
    // given:
    mem[0xFF00] = CPU::INS_JMP_ABS;
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    constexpr s32 EXPECTED_CYCLES = 3;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC, 0x8000);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
    EXPECT_EQ(cpu.SP, cpuCopy.SP);
}

TEST_F(JumpsAndCallsTests, JumpIndirectCanJumpToNewLocation) {
    // given:
    mem[0xFF00] = CPU::INS_JMP_IND;
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    mem[0x8000] = 0x00; // 0x9000
    mem[0x8001] = 0x90;
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC, 0x9000);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
    EXPECT_EQ(cpu.SP, cpuCopy.SP);
}
