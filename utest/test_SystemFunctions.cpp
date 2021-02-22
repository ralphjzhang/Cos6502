#include <gtest/gtest.h>

#include "../core/cp6502.hpp"

using namespace cp6502;

struct SystemFunctionTests : public testing::Test {
    Mem mem;
    CPU cpu;

    virtual void SetUp() {
        cpu.Reset(0xFF00, mem);
    }

    virtual void TearDown() {
    }

    void ExpectUnaffectedRegisters(CPU const& cpuCopy) {
        EXPECT_EQ(cpu.C, cpuCopy.C);
        EXPECT_EQ(cpu.Z, cpuCopy.Z);
        EXPECT_EQ(cpu.I, cpuCopy.I);
        EXPECT_EQ(cpu.D, cpuCopy.D);
        EXPECT_EQ(cpu.V, cpuCopy.V);
        EXPECT_EQ(cpu.N, cpuCopy.N);
    }
};

TEST_F(SystemFunctionTests, BRKPushesPCAndPS) {
    // given:
    cpu.Z = true;
    cpu.C = true;
    mem[0xFF00] = CPU::INS_BRK;
    mem[0xFFFE] = 0x00;
    mem[0xFFFF] = 0x90;
    constexpr s32 EXPECTED_CYCLES = 7;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.SP, 0xFF-3);
    EXPECT_EQ(mem[cpu.SPToAddress()+1], cpuCopy.PS);
    EXPECT_EQ(mem[cpu.SPToAddress()+2], 0x01);
    EXPECT_EQ(mem[cpu.SPToAddress()+3], 0xFF); // pushed PC: 0xFF01
    ExpectUnaffectedRegisters(cpuCopy);
}

TEST_F(SystemFunctionTests, BRKLoadsPCAndSetBFlag) {
    // given:
    mem[0xFF00] = CPU::INS_BRK;
    mem[0xFFFE] = 0x00;
    mem[0xFFFF] = 0x90;
    constexpr s32 EXPECTED_CYCLES = 7;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PC, 0x9000);
    EXPECT_TRUE(cpu.B);
    ExpectUnaffectedRegisters(cpuCopy);
}

TEST_F(SystemFunctionTests, RTIPullsPSAndPC) {
    // given:
    cpu.Z = true;
    cpu.C = true;
    mem[0xFF00] = CPU::INS_BRK;
    mem[0xFFFE] = 0x00;
    mem[0xFFFF] = 0x80;
    mem[0x8000] = CPU::INS_RTI;
    constexpr s32 EXPECTED_CYCLES = 7 + 6;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpuCopy.SP, cpu.SP);
    EXPECT_EQ(cpuCopy.PS, cpu.PS);
    EXPECT_EQ(cpuCopy.PC+1, cpu.PC);
}


TEST_F(SystemFunctionTests, NOP) {
    // given:
    mem[0xFF00] = CPU::INS_NOP;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
    EXPECT_EQ(cpu.PC, cpuCopy.PC + 1);
    EXPECT_EQ(cpu.A, cpuCopy.A);
    EXPECT_EQ(cpu.X, cpuCopy.X);
    EXPECT_EQ(cpu.Y, cpuCopy.Y);
}
