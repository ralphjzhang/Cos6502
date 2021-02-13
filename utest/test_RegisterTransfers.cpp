#include <gtest/gtest.h>

#include "../core/cp6502.hpp"

using namespace cp6502;

struct RegisterTransferTests : public testing::Test {
    Mem mem;
    CPU cpu;

    virtual void SetUp() {
        cpu.Reset(0xFFFC, mem);
    }

    virtual void TearDown() {
    }

    void ExpectUnaffectedRegisters(CPU const& cpuCopy) {
        EXPECT_EQ(cpuCopy.C, cpu.C);
        EXPECT_EQ(cpuCopy.I, cpu.I);
        EXPECT_EQ(cpuCopy.D, cpu.D);
        EXPECT_EQ(cpuCopy.B, cpu.B);
        EXPECT_EQ(cpuCopy.V, cpu.V);
    }

    void TestTransferPositiveValue(Byte opcode, Byte CPU::*dstRegister, Byte CPU::*srcRegister) {
        // given:
        cpu.*srcRegister = 0x42;
        cpu.*dstRegister = 0x00;
        cpu.Z = cpu.N = true;
        mem[0xFFFC] = opcode;
        constexpr s32 EXPECTED_CYCLES = 2;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(cpu.*dstRegister, cpuCopy.*srcRegister);
        EXPECT_EQ(cpu.*srcRegister, cpuCopy.*srcRegister);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    void TestTransferZeroValue(Byte opcode, Byte CPU::*dstRegister, Byte CPU::*srcRegister) {
        // given:
        cpu.*srcRegister = 0x00;
        cpu.*dstRegister = 0xFF;
        cpu.Z = false;
        cpu.N = true;
        mem[0xFFFC] = opcode;
        constexpr s32 EXPECTED_CYCLES = 2;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(cpu.*dstRegister, cpuCopy.*srcRegister);
        EXPECT_EQ(cpu.*srcRegister, cpuCopy.*srcRegister);
        EXPECT_TRUE(cpu.Z);
        EXPECT_FALSE(cpu.N);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    void TestTransferNegativeValue(Byte opcode, Byte CPU::*dstRegister, Byte CPU::*srcRegister) {
        // given:
        cpu.*srcRegister = 0b11011101;
        cpu.*dstRegister = 0x00;
        cpu.Z = true;
        cpu.N = false;
        mem[0xFFFC] = opcode;
        constexpr s32 EXPECTED_CYCLES = 2;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(cpu.*dstRegister, cpuCopy.*srcRegister);
        EXPECT_EQ(cpu.*srcRegister, cpuCopy.*srcRegister);
        EXPECT_FALSE(cpu.Z);
        EXPECT_TRUE(cpu.N);
        ExpectUnaffectedRegisters(cpuCopy);
    }
};

TEST_F(RegisterTransferTests, TAXCanTransferPositiveValue) {
    TestTransferPositiveValue(CPU::INS_TAX, &CPU::X, &CPU::A);
}

TEST_F(RegisterTransferTests, TAYCanTransferPositiveValue) {
    TestTransferPositiveValue(CPU::INS_TAY, &CPU::Y, &CPU::A);
}

TEST_F(RegisterTransferTests, TXACanTransferPositiveValue) {
    TestTransferPositiveValue(CPU::INS_TXA, &CPU::A, &CPU::X);
}

TEST_F(RegisterTransferTests, TYACanTransferPositiveValue) {
    TestTransferPositiveValue(CPU::INS_TYA, &CPU::A, &CPU::Y);
}

TEST_F(RegisterTransferTests, TAXCanTransferNegativeValue) {
    TestTransferNegativeValue(CPU::INS_TAX, &CPU::X, &CPU::A);
}

TEST_F(RegisterTransferTests, TAYCanTransferNegativeValue) {
    TestTransferNegativeValue(CPU::INS_TAY, &CPU::Y, &CPU::A);
}

TEST_F(RegisterTransferTests, TXACanTransferNegativeValue) {
    TestTransferNegativeValue(CPU::INS_TXA, &CPU::A, &CPU::X);
}

TEST_F(RegisterTransferTests, TYACanTransferNegativeValue) {
    TestTransferNegativeValue(CPU::INS_TYA, &CPU::A, &CPU::Y);
}

TEST_F(RegisterTransferTests, TAXCanTransferZeroValue) {
    TestTransferZeroValue(CPU::INS_TAX, &CPU::X, &CPU::A);
}

TEST_F(RegisterTransferTests, TAYCanTransferZeroValue) {
    TestTransferZeroValue(CPU::INS_TAY, &CPU::Y, &CPU::A);
}

TEST_F(RegisterTransferTests, TXACanTransferZeroValue) {
    TestTransferZeroValue(CPU::INS_TXA, &CPU::A, &CPU::X);
}

TEST_F(RegisterTransferTests, TYACanTransferZeroValue) {
    TestTransferZeroValue(CPU::INS_TYA, &CPU::A, &CPU::Y);
}

