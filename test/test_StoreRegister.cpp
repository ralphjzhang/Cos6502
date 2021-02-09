#include <gtest/gtest.h>

#include "../core/cos6502.hpp"

using namespace cos6502;

struct StoreRegisterTests : public testing::Test {
    Mem mem;
    CPU cpu;

    virtual void SetUp() {
        cpu.Reset(0xFFFC, mem);
    }

    virtual void TearDown() {
    }

    void VerifyUnmodifiedFlagsFromLoadRegister(CPU const& cpu, CPU const& cpuCopy) {
        EXPECT_EQ(cpu.C, cpuCopy.C);
        EXPECT_EQ(cpu.I, cpuCopy.I);
        EXPECT_EQ(cpu.D, cpuCopy.D);
        EXPECT_EQ(cpu.B, cpuCopy.B);
        EXPECT_EQ(cpu.V, cpuCopy.V);
        EXPECT_EQ(cpu.Z, cpuCopy.Z);
        EXPECT_EQ(cpu.N, cpuCopy.N);
    }

    void TestStoreRegisterZeroPage(Byte opcode, Byte CPU::*Register) {
        // given:
        cpu.*Register = 0x2F;
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x80;
        mem[0x0080] = 0x00;
        constexpr s32 EXPECTED_CYCLES = 3;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
        EXPECT_EQ(mem[0x0080], 0x2F);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }

    void TestStoreRegisterZeroPageXY(Byte opcode, Byte CPU::*RegisterToAdd, Byte CPU::*Register) {
        // given:
        cpu.*RegisterToAdd = 0x0F;
        cpu.*Register = 0x42;
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x80;
        mem[0x008F] = 0x00;
        constexpr s32 EXPECTED_CYCLES = 4;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
        EXPECT_EQ(mem[0x008F], 0x42);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }

    void TestStoreRegisterAbsolute(Byte opcode, Byte CPU::*Register) {
        // given:
        cpu.*Register = 0x2F;
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x00;
        mem[0xFFFE] = 0x80; // 0x8000
        mem[0x8000] = 0x00;
        constexpr s32 EXPECTED_CYCLES = 4;
        CPU cpuCopy = cpu;
        // when:
        s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
        EXPECT_EQ(mem[0x8000], 0x2F);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }

    void TestStoreRegisterAbsoluteXY(Byte opcode, Byte CPU::*RegisterToAdd) {
        // given:
        cpu.*RegisterToAdd = 0x92;
        cpu.A = 0x42;
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x00;
        mem[0xFFFE] = 0x20; // 0x2000
        mem[0x2092] = 0x00;
        constexpr s32 EXPECTED_CYCLES = 5;
        CPU cpuCopy = cpu;
        // when:
        s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
        EXPECT_EQ(mem[0x2092], 0x42);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }

    void TestStoreRegisterIndirectXY(Byte opcode, Byte CPU::*RegisterToAdd) {
        // given:
        cpu.*RegisterToAdd = 0x0F;
        cpu.A = 0x42;
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x20;
        mem[0x0020] = 0x00;
        mem[0x0021] = 0x80; // 0x8000
        mem[0x800F] = 0x00;
        constexpr s32 EXPECTED_CYCLES = 6;
        CPU cpuCopy = cpu;
        // when:
        s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
        EXPECT_EQ(mem[0x800F], 0x42);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }
};

TEST_F(StoreRegisterTests, STAZeroPageCanStoreARegisterIntoMemory) {
    TestStoreRegisterZeroPage(CPU::INS_STA_ZP, &CPU::A);
}

TEST_F(StoreRegisterTests, STXZeroPageCanStoreXRegisterIntoMemory) {
    TestStoreRegisterZeroPage(CPU::INS_STX_ZP, &CPU::X);
}

TEST_F(StoreRegisterTests, STYZeroPageCanStoreYRegisterIntoMemory) {
    TestStoreRegisterZeroPage(CPU::INS_STY_ZP, &CPU::Y);
}

TEST_F(StoreRegisterTests, STAZeroPageXCanStoreARegisterIntoMemory) {
    TestStoreRegisterZeroPageXY(CPU::INS_STA_ZPX, &CPU::X, &CPU::A);
}

TEST_F(StoreRegisterTests, STXZeroPageYCanStoreXRegisterIntoMemory) {
    TestStoreRegisterZeroPageXY(CPU::INS_STX_ZPY, &CPU::Y, &CPU::X);
}

TEST_F(StoreRegisterTests, STYZeroPageXCanStoreYRegisterIntoMemory) {
    TestStoreRegisterZeroPageXY(CPU::INS_STY_ZPX, &CPU::X, &CPU::Y);
}

TEST_F(StoreRegisterTests, STAAbsoluteCanStoreARegisterIntoMemory) {
    TestStoreRegisterAbsolute(CPU::INS_STA_ABS, &CPU::A);
}

TEST_F(StoreRegisterTests, STXAbsoluteCanStoreXRegisterIntoMemory) {
    TestStoreRegisterAbsolute(CPU::INS_STX_ABS, &CPU::X);
}

TEST_F(StoreRegisterTests, STYAbsoluteCanStoreYRegisterIntoMemory) {
    TestStoreRegisterAbsolute(CPU::INS_STY_ABS, &CPU::Y);
}

TEST_F(StoreRegisterTests, STAAbsoluteXCanStoreARegisterIntoMemory) {
    TestStoreRegisterAbsoluteXY(CPU::INS_STA_ABSX, &CPU::X);
}

TEST_F(StoreRegisterTests, STAAbsoluteYCanStoreARegisterIntoMemory) {
    TestStoreRegisterAbsoluteXY(CPU::INS_STA_ABSY, &CPU::Y);
}

TEST_F(StoreRegisterTests, STAIndirectXCanStoreARegisterIntoMemory) {
    // given:
    cpu.X = 0x0F;
    cpu.A = 0x42;
    mem[0xFFFC] = CPU::INS_STA_INDX;
    mem[0xFFFD] = 0x20;
    mem[0x002F] = 0x00; // 0x20 + 0x0F
    mem[0x0030] = 0x80;
    mem[0x8000] = 0x00;
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU cpuCopy = cpu;
    // when:
    s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(mem[0x8000], 0x42);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);

}

TEST_F(StoreRegisterTests, STAIndirectYCanStoreARegisterIntoMemory) {
    // given:
    cpu.Y = 0x0F;
    cpu.A = 0x42;
    mem[0xFFFC] = CPU::INS_STA_INDY;
    mem[0xFFFD] = 0x20;
    mem[0x0020] = 0x00;
    mem[0x0021] = 0x80; // 0x8000
    mem[0x800F] = 0x00;
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU cpuCopy = cpu;
    // when:
    s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(mem[0x800F], 0x42);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);

}
