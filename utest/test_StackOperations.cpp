#include <gtest/gtest.h>

#include "../core/cp6502.hpp"

using namespace cp6502;

struct StackOperations : public testing::Test {
    Mem mem;
    CPU cpu;

    virtual void SetUp() {
        cpu.Reset(0xFF00, mem);
    }

    virtual void TearDown() {
    }
};

TEST_F(StackOperations, TSX) {
    // given:
    cpu.X = 0x00;
    cpu.Z = cpu.N = true;
    cpu.SP = 0x01;
    mem[0xFF00] = CPU::INS_TSX;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.X, 0x01);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(StackOperations, TSXCanTransferAZeroStackPointerToXRegister) {
    // given:
    cpu.X = 0x01;
    cpu.Z = false;
    cpu.N = true;
    cpu.SP = 0x00;
    mem[0xFF00] = CPU::INS_TSX;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.X, 0x00);
    EXPECT_TRUE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(StackOperations, TSXCanTransferANegativeStackPointerToXRegister) {
    // given:
    cpu.X = 0x00;
    cpu.Z = true;
    cpu.N = false;
    cpu.SP = 0b10000000;
    mem[0xFF00] = CPU::INS_TSX;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.X, 0b10000000);
    EXPECT_FALSE(cpu.Z);
    EXPECT_TRUE(cpu.N);
}

TEST_F(StackOperations, TXS) {
    // given:
    cpu.X = 0xFF;
    cpu.SP = 0;
    mem[0xFF00] = CPU::INS_TXS;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.SP, 0xFF);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
}

TEST_F(StackOperations, PHACanPushARegisterOntoStack) {
    // given:
    cpu.A = 0x42;
    mem[0xFF00] = CPU::INS_PHA;
    constexpr s32 EXPECTED_CYCLES = 3;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(mem[cpu.SPToAddress() + 1], cpu.A);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
    EXPECT_EQ(cpu.SP, 0xFE);
}

TEST_F(StackOperations, PHPCanPushStatusRegistersOntoStack) {
    // given:
    cpu.PS = 0xCC;
    mem[0xFF00] = CPU::INS_PHP;
    constexpr s32 EXPECTED_CYCLES = 3;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(mem[cpu.SPToAddress() + 1], cpu.PS | BreakFlag | UnusedFlag);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
    EXPECT_EQ(cpu.SP, 0xFE);
}

TEST_F(StackOperations, PHPSetBit4and5OnStack) {
    // given:
    cpu.PS = 0x00;
    mem[0xFF00] = CPU::INS_PHP;
    constexpr s32 EXPECTED_CYCLES = 3;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    Word AddPSOnStack = cpu.SPToAddress() + 1;
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    // struct {
    //     Byte C : 1;         // 0: Carry
    //     Byte Z : 1;         // 1: Zero
    //     Byte I : 1;         // 2: Interrupe
    //     Byte D : 1;         // 3: Decimal
    //     Byte B : 1;         // 4: Break
    //     Byte Unused : 1;    // 5: Unused
    //     Byte V : 1;         // 6: Overflow
    //     Byte N : 1;         // 7: Negative
    // };
    const Byte FlagsOnStack = 0b00110000;
    EXPECT_EQ(mem[AddPSOnStack], FlagsOnStack);
}

TEST_F(StackOperations, PLACanPullFromStackToARegister) {
    // given:
    cpu.A = 0x00;
    cpu.SP = 0xFE;
    mem[0xFF00] = CPU::INS_PLA;
    mem[0x01FF] = 0x42;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_EQ(cpu.SP, 0xFF);
}

TEST_F(StackOperations, PLACanPullAZeroValueFromStackIntoRegister) {
    // given:
    cpu.Z = false;
    cpu.N = true;
    cpu.A = 0x42;
    cpu.SP = 0xFE;
    mem[0xFF00] = CPU::INS_PLA;
    mem[0x01FF] = 0x00;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_TRUE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    EXPECT_EQ(cpu.SP, 0xFF);
}

TEST_F(StackOperations, PLACanPullANegativeValueFromStackIntoRegister) {
    // given:
    cpu.Z = true;
    cpu.N = false;
    cpu.A = 0x00;
    cpu.SP = 0xFE;
    mem[0xFF00] = CPU::INS_PLA;
    mem[0x01FF] = 0b10000000;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.A, 0b10000000);
    EXPECT_FALSE(cpu.Z);
    EXPECT_TRUE(cpu.N);
    EXPECT_EQ(cpu.SP, 0xFF);
}

TEST_F(StackOperations, PLPPullPSFromStackIgnoreBit4And5) {
    // given:
    cpu.SP = 0xFE;
    cpu.PS = 0x00;
    mem[0xFF00] = CPU::INS_PLP;
    mem[0x01FF] = 0x42 | BreakFlag | UnusedFlag;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PS, 0x42);
    EXPECT_EQ(cpu.SP, 0xFF);
}

TEST_F(StackOperations, PLPClearBit4And5) {
    // given:
    cpu.SP = 0xFE;
    cpu.PS = BreakFlag | UnusedFlag;
    mem[0xFF00] = CPU::INS_PLP;
    mem[0x01FF] = 0;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PS, 0);
}

