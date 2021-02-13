#include <gtest/gtest.h>

#include "../core/cp6502.hpp"

using namespace cp6502;

struct IncrementDecrementTests : public testing::Test {
    Mem mem;
    CPU cpu;

    virtual void SetUp() {
        cpu.Reset(0xFF00, mem);
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

    void TestIncrementFromZeroValue(Byte opcode, Byte CPU::*reg) {
        // given:
        cpu.*reg = 0;
        cpu.Z = true;
        cpu.N = true;
        mem[0xFF00] = opcode;
        constexpr s32 EXPECTED_CYCLES = 2;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(cpu.*reg, 1);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    void TestDecrementFromZeroValue(Byte opcode, Byte CPU::*reg) {
        // given:
        cpu.*reg = 0;
        cpu.Z = true;
        cpu.N = false;
        mem[0xFF00] = opcode;
        constexpr s32 EXPECTED_CYCLES = 2;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(cpu.*reg, 0xFF);
        EXPECT_FALSE(cpu.Z);
        EXPECT_TRUE(cpu.N);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    void TestToZeroValue(Byte opcode, Byte CPU::*reg, Byte fromValue) {
        // given:
        cpu.*reg = fromValue;
        cpu.Z = false;
        cpu.N = true;
        mem[0xFF00] = opcode;
        constexpr s32 EXPECTED_CYCLES = 2;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(cpu.*reg, 0);
        EXPECT_TRUE(cpu.Z);
        EXPECT_FALSE(cpu.N);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    void TestToZeroValueZeroPage(Byte opcode, Byte fromValue) {
        // given:
        cpu.Z = false;
        cpu.N = true;
        mem[0xFF00] = opcode;
        mem[0xFF01] = 0x42;
        mem[0x0042] = fromValue;
        constexpr s32 EXPECTED_CYCLES = 5;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(mem[0x0042], 0);
        EXPECT_TRUE(cpu.Z);
        EXPECT_FALSE(cpu.N);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    void TestToZeroValueZeroPageX(Byte opcode, Byte fromValue) {
        // given:
        cpu.Z = false;
        cpu.N = true;
        cpu.X = 0x0F;
        mem[0xFF00] = opcode;
        mem[0xFF01] = 0x80;
        mem[0x008F] = fromValue;
        constexpr s32 EXPECTED_CYCLES = 6;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(mem[0x008F], 0);
        EXPECT_TRUE(cpu.Z);
        EXPECT_FALSE(cpu.N);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    void TestToZeroValueAbsolute(Byte opcode, Byte fromValue) {
        // given:
        cpu.Z = false;
        cpu.N = true;
        mem[0xFF00] = opcode;
        mem[0xFF01] = 0x00;
        mem[0xFF02] = 0x80;
        mem[0x8000] = fromValue;
        constexpr s32 EXPECTED_CYCLES = 6;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(mem[0x8000], 0);
        EXPECT_TRUE(cpu.Z);
        EXPECT_FALSE(cpu.N);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    void TestToZeroValueAbsoluteX(Byte opcode, Byte fromValue) {
        // given:
        cpu.Z = false;
        cpu.N = true;
        cpu.X = 0x01;
        mem[0xFF00] = opcode;
        mem[0xFF01] = 0x00;
        mem[0xFF02] = 0x80;
        mem[0x8001] = fromValue;
        constexpr s32 EXPECTED_CYCLES = 7;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(mem[0x8001], 0);
        EXPECT_TRUE(cpu.Z);
        EXPECT_FALSE(cpu.N);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    void TestToPositiveValueZeroPage(Byte opcode, Byte fromValue, Byte toValue) {
        // given:
        cpu.Z = true;
        cpu.N = true;
        mem[0xFF00] = opcode;
        mem[0xFF01] = 0x42;
        mem[0x0042] = fromValue;
        constexpr s32 EXPECTED_CYCLES = 5;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(mem[0x0042], toValue);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    void TestToPositiveValueZeroPageX(Byte opcode, Byte fromValue, Byte toValue) {
        // given:
        cpu.Z = true;
        cpu.N = true;
        cpu.X = 0x0F;
        mem[0xFF00] = opcode;
        mem[0xFF01] = 0x80;
        mem[0x008F] = fromValue;
        constexpr s32 EXPECTED_CYCLES = 6;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(mem[0x008F], toValue);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    void TestToPositiveValueAbsolute(Byte opcode, Byte fromValue, Byte toValue) {
        // given:
        cpu.Z = true;
        cpu.N = true;
        mem[0xFF00] = opcode;
        mem[0xFF01] = 0x00;
        mem[0xFF02] = 0x80;
        mem[0x8000] = fromValue;
        constexpr s32 EXPECTED_CYCLES = 6;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(mem[0x8000], toValue);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    void TestToPositiveValueAbsoluteX(Byte opcode, Byte fromValue, Byte toValue) {
        // given:
        cpu.Z = true;
        cpu.N = true;
        cpu.X = 0x01;
        mem[0xFF00] = opcode;
        mem[0xFF01] = 0x00;
        mem[0xFF02] = 0x80;
        mem[0x8001] = fromValue;
        constexpr s32 EXPECTED_CYCLES = 7;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(mem[0x8001], toValue);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    void TestToNegativeValueZeroPage(Byte opcode, Byte fromValue, Byte toValue) {
        // given:
        cpu.Z = true;
        cpu.N = false;
        mem[0xFF00] = opcode;
        mem[0xFF01] = 0x42;
        mem[0x0042] = fromValue;
        constexpr s32 EXPECTED_CYCLES = 5;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(mem[0x0042], toValue);
        EXPECT_FALSE(cpu.Z);
        EXPECT_TRUE(cpu.N);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    void TestToNegativeValueZeroPageX(Byte opcode, Byte fromValue, Byte toValue) {
        // given:
        cpu.Z = true;
        cpu.N = false;
        cpu.X = 0x0F;
        mem[0xFF00] = opcode;
        mem[0xFF01] = 0x80;
        mem[0x008F] = fromValue;
        constexpr s32 EXPECTED_CYCLES = 6;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(mem[0x008F], toValue);
        EXPECT_FALSE(cpu.Z);
        EXPECT_TRUE(cpu.N);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    void TestToNegativeValueAbsolute(Byte opcode, Byte fromValue, Byte toValue) {
        // given:
        cpu.Z = true;
        cpu.N = false;
        mem[0xFF00] = opcode;
        mem[0xFF01] = 0x00;
        mem[0xFF02] = 0x80;
        mem[0x8000] = fromValue;
        constexpr s32 EXPECTED_CYCLES = 6;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(mem[0x8000], toValue);
        EXPECT_FALSE(cpu.Z);
        EXPECT_TRUE(cpu.N);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    void TestToNegativeValueAbsoluteX(Byte opcode, Byte fromValue, Byte toValue) {
        // given:
        cpu.Z = true;
        cpu.N = false;
        cpu.X = 0x01;
        mem[0xFF00] = opcode;
        mem[0xFF01] = 0x00;
        mem[0xFF02] = 0x80;
        mem[0x8001] = fromValue;
        constexpr s32 EXPECTED_CYCLES = 7;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(mem[0x8001], toValue);
        EXPECT_FALSE(cpu.Z);
        EXPECT_TRUE(cpu.N);
        ExpectUnaffectedRegisters(cpuCopy);
    }
};

TEST_F(IncrementDecrementTests, INXFromZeroValue) {
    TestIncrementFromZeroValue(CPU::INS_INX, &CPU::X);
}

TEST_F(IncrementDecrementTests, INYFromZeroValue) {
    TestIncrementFromZeroValue(CPU::INS_INY, &CPU::Y);
}

TEST_F(IncrementDecrementTests, DEXFromZeroValue) {
    TestDecrementFromZeroValue(CPU::INS_DEX, &CPU::X);
}

TEST_F(IncrementDecrementTests, DEYFromZeroValue) {
    TestDecrementFromZeroValue(CPU::INS_DEY, &CPU::Y);
}

TEST_F(IncrementDecrementTests, INXToZeroValue) {
    TestToZeroValue(CPU::INS_INX, &CPU::X, 0xFF);
}

TEST_F(IncrementDecrementTests, INYToZeroValue) {
    TestToZeroValue(CPU::INS_INY, &CPU::Y, 0xFF);
}

TEST_F(IncrementDecrementTests, DEXToZeroValue) {
    TestToZeroValue(CPU::INS_DEX, &CPU::X, 0x01);
}

TEST_F(IncrementDecrementTests, DEYToZeroValue) {
    TestToZeroValue(CPU::INS_DEY, &CPU::Y, 0x01);
}

TEST_F(IncrementDecrementTests, INCZeroPageToZeroValue) {
    TestToZeroValueZeroPage(CPU::INS_INC_ZP, 0xFF);
}

TEST_F(IncrementDecrementTests, DECZeroPageToZeroValue) {
    TestToZeroValueZeroPage(CPU::INS_DEC_ZP, 0x01);
}

TEST_F(IncrementDecrementTests, INCZeroPageToPositiveValue) {
    TestToPositiveValueZeroPage(CPU::INS_INC_ZP, 0x10, 0x11);
}

TEST_F(IncrementDecrementTests, DECZeroPageToPositiveValue) {
    TestToPositiveValueZeroPage(CPU::INS_DEC_ZP, 0x10, 0x0F);
}

TEST_F(IncrementDecrementTests, INCZeroPageToNegativeValue) {
    TestToNegativeValueZeroPage(CPU::INS_INC_ZP, 0xF1, 0xF2);
}

TEST_F(IncrementDecrementTests, DECZeroPageToNegativeValue) {
    TestToNegativeValueZeroPage(CPU::INS_DEC_ZP, 0xF1, 0xF0);
}

TEST_F(IncrementDecrementTests, INCZeroPageXToZeroValue) {
    TestToZeroValueZeroPageX(CPU::INS_INC_ZPX, 0xFF);
}

TEST_F(IncrementDecrementTests, DECZeroPageXToZeroValue) {
    TestToZeroValueZeroPageX(CPU::INS_DEC_ZPX, 0x01);
}

TEST_F(IncrementDecrementTests, INCZeroPageXToPositiveValue) {
    TestToPositiveValueZeroPageX(CPU::INS_INC_ZPX, 0x10, 0x11);
}

TEST_F(IncrementDecrementTests, DECZeroPageXToPositiveValue) {
    TestToPositiveValueZeroPageX(CPU::INS_DEC_ZPX, 0x10, 0x0F);
}

TEST_F(IncrementDecrementTests, INCZeroPageXToNegativeValue) {
    TestToNegativeValueZeroPageX(CPU::INS_INC_ZPX, 0xF1, 0xF2);
}

TEST_F(IncrementDecrementTests, DECZeroPageXToNegativeValue) {
    TestToNegativeValueZeroPageX(CPU::INS_DEC_ZPX, 0xF1, 0xF0);
}

TEST_F(IncrementDecrementTests, INCAbsoluteToZeroValue) {
    TestToZeroValueAbsolute(CPU::INS_INC_ABS, 0xFF);
}

TEST_F(IncrementDecrementTests, DECAbsoluteToZeroValue) {
    TestToZeroValueAbsolute(CPU::INS_DEC_ABS, 0x01);
}

TEST_F(IncrementDecrementTests, INCAbsoluteToPositiveValue) {
    TestToPositiveValueAbsolute(CPU::INS_INC_ABS, 0x10, 0x11);
}

TEST_F(IncrementDecrementTests, DECAbsoluteToPositiveValue) {
    TestToPositiveValueAbsolute(CPU::INS_DEC_ABS, 0x10, 0x0F);
}

TEST_F(IncrementDecrementTests, INCAbsoluteToNegativeValue) {
    TestToNegativeValueAbsolute(CPU::INS_INC_ABS, 0xF1, 0xF2);
}

TEST_F(IncrementDecrementTests, DECAbsoluteToNegativeValue) {
    TestToNegativeValueAbsolute(CPU::INS_DEC_ABS, 0xF1, 0xF0);
}

TEST_F(IncrementDecrementTests, INCAbsoluteXToZeroValue) {
    TestToZeroValueAbsoluteX(CPU::INS_INC_ABSX, 0xFF);
}

TEST_F(IncrementDecrementTests, DECAbsoluteXToZeroValue) {
    TestToZeroValueAbsoluteX(CPU::INS_DEC_ABSX, 0x01);
}

TEST_F(IncrementDecrementTests, INCAbsoluteXToPositiveValue) {
    TestToPositiveValueAbsoluteX(CPU::INS_INC_ABSX, 0x10, 0x11);
}

TEST_F(IncrementDecrementTests, DECAbsoluteXToPositiveValue) {
    TestToPositiveValueAbsoluteX(CPU::INS_DEC_ABSX, 0x10, 0x0F);
}

TEST_F(IncrementDecrementTests, INCAbsoluteXToNegativeValue) {
    TestToNegativeValueAbsoluteX(CPU::INS_INC_ABSX, 0xF1, 0xF2);
}

TEST_F(IncrementDecrementTests, DECAbsoluteXToNegativeValue) {
    TestToNegativeValueAbsoluteX(CPU::INS_DEC_ABSX, 0xF1, 0xF0);
}

TEST_F(IncrementDecrementTests, TestLoadProgramAndExecute) {
    // given:
    /*
        * = $FF00
        lda #00
        sta $42

        start
        inc $42
        ldx $42
        inx 
        jmp start
    */

    constexpr static s32 NumByteTestProg = 14;
    Byte TestProg[NumByteTestProg] = { 0x00,0xFF,0xA9,0x00,0x85,0x42,0xE6,0x42,0xA6,0x42,0xE8,0x4C,0x04,0xFF };
    // when:
    cpu.PC = cpu.LoadProg(TestProg, NumByteTestProg, mem);
    for (s32 clock = 20; clock > 0; --clock)
        clock -= cpu.Execute(1, mem);
    // then;
    EXPECT_EQ(cpu.A, 0);
    EXPECT_EQ(mem[0x42], 1);
    EXPECT_EQ(cpu.X, 2);
}

