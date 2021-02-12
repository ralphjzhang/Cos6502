#include <gtest/gtest.h>

#include "../core/cp6502.hpp"

using namespace cp6502;

struct LogicalTests : public testing::Test {
    Mem mem;
    CPU cpu;

    virtual void SetUp() {
        cpu.Reset(0xFFFC, mem);
    }

    virtual void TearDown() {
    }

    void VerifyUnmodifiedFlagsFromLoadRegister(const CPU& cpu, const CPU& cpuCopy) {
        EXPECT_EQ(cpu.C, cpuCopy.C);
        EXPECT_EQ(cpu.I, cpuCopy.I);
        EXPECT_EQ(cpu.D, cpuCopy.D);
        EXPECT_EQ(cpu.B, cpuCopy.B);
        EXPECT_EQ(cpu.V, cpuCopy.V);
    }

    template <typename T>
    void TestLogicalOnARegisterImmediate(Byte opcode, T logicalOp) {
        // given:
        cpu.A = 0xCC;
        cpu.Z = true;
        cpu.N = true;
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x84;
        CPU cpuCopy = cpu;
        constexpr s32 EXPECTED_CYCLES = 2;
        // when:
        s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        Byte resultA = logicalOp(cpuCopy.A, mem[0xFFFD]);
        EXPECT_EQ(cpu.A, resultA);
        EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
        if (resultA == 0)
            EXPECT_TRUE(cpu.Z);
        else
            EXPECT_FALSE(cpu.Z);
        if (resultA >> 7 == 0x01)
            EXPECT_TRUE(cpu.N);
        else
            EXPECT_FALSE(cpu.N);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }

    template <typename T>
    void TestLogicalOnARegisterZeroPage(Byte opcode, T logicalOp) {
        // given:
        cpu.A = 0xCC;
        cpu.Z = true;
        cpu.N = true;
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x42;
        mem[0x0042] = 0x84;
        CPU cpuCopy = cpu;
        constexpr s32 EXPECTED_CYCLES = 3;
        // when:
        s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        Byte resultA = logicalOp(cpuCopy.A, mem[0x0042]);
        EXPECT_EQ(cpu.A, resultA);
        EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
        if (resultA == 0)
            EXPECT_TRUE(cpu.Z);
        else
            EXPECT_FALSE(cpu.Z);
        if (resultA >> 7 == 0x01)
            EXPECT_TRUE(cpu.N);
        else
            EXPECT_FALSE(cpu.N);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }

    template <typename T>
    void TestLogicalOnARegisterZeroPageX(Byte opcode, T logicalOp) {
        // given:
        cpu.A = 0xCC;
        cpu.X = 5;
        cpu.Z = true;
        cpu.N = true;
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x42;
        mem[0x0047] = 0x84;
        CPU cpuCopy = cpu;
        constexpr s32 EXPECTED_CYCLES = 4;
        // when:
        s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        Byte resultA = logicalOp(cpuCopy.A, mem[0x0047]);
        EXPECT_EQ(cpu.A, resultA);
        EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
        if (resultA == 0)
            EXPECT_TRUE(cpu.Z);
        else
            EXPECT_FALSE(cpu.Z);
        if (resultA >> 7 == 0x01)
            EXPECT_TRUE(cpu.N);
        else
            EXPECT_FALSE(cpu.N);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }

    template <typename T>
    void TestLogicalOnARegisterAbsolute(Byte opcode, T logicalOp) {
        // given:
        cpu.A = 0xCC;
        cpu.Z = true;
        cpu.N = true;
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x80;
        mem[0xFFFE] = 0x44; // 0x4480
        mem[0x4480] = 0x84;
        CPU cpuCopy = cpu;
        constexpr s32 EXPECTED_CYCLES = 4;
        // when:
        s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        Byte resultA = logicalOp(cpuCopy.A, mem[0x4480]);
        EXPECT_EQ(cpu.A, resultA);
        EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
        if (resultA == 0)
            EXPECT_TRUE(cpu.Z);
        else
            EXPECT_FALSE(cpu.Z);
        if (resultA >> 7 == 0x01)
            EXPECT_TRUE(cpu.N);
        else
            EXPECT_FALSE(cpu.N);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }

    template <typename T>
    void TestLogicalOnARegisterAbsoluteXY(Byte opcode, Byte CPU::*RegisterToAdd, T logicalOp) {
        // given:
        cpu.A = 0xCC;
        cpu.*RegisterToAdd = 1;
        cpu.Z = true;
        cpu.N = true;
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x80;
        mem[0xFFFE] = 0x44; // 0x4480
        mem[0x4481] = 0x84;
        CPU cpuCopy = cpu;
        constexpr s32 EXPECTED_CYCLES = 4;
        // when:
        s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        Byte resultA = logicalOp(cpuCopy.A, mem[0x4481]);
        EXPECT_EQ(cpu.A, resultA);
        EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
        if (resultA == 0)
            EXPECT_TRUE(cpu.Z);
        else
            EXPECT_FALSE(cpu.Z);
        if (resultA >> 7 == 0x01)
            EXPECT_TRUE(cpu.N);
        else
            EXPECT_FALSE(cpu.N);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }

    template <typename T>
    void TestLogicalOnARegisterAbsoluteXYWhenCrossPage(Byte opcode, Byte CPU::*RegisterToAdd, T logicalOp) {
        // given:
        cpu.A = 0xCC;
        cpu.*RegisterToAdd = 0xFF;
        cpu.Z = true;
        cpu.N = true;
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x02;
        mem[0xFFFE] = 0x44; // 0x4402
        mem[0x4501] = 0x84; // 0x4402 + 0x00FF
        CPU cpuCopy = cpu;
        constexpr s32 EXPECTED_CYCLES = 5;
        // when:
        s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        Byte resultA = logicalOp(cpuCopy.A, mem[0x4501]);
        EXPECT_EQ(cpu.A, resultA);
        EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
        if (resultA == 0)
            EXPECT_TRUE(cpu.Z);
        else
            EXPECT_FALSE(cpu.Z);
        if (resultA >> 7 == 0x01)
            EXPECT_TRUE(cpu.N);
        else
            EXPECT_FALSE(cpu.N);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }

    template <typename T>
    void TestLogicalOnARegisterIndirectX(Byte opcode, T logicalOp) {
        // given:
        cpu.A = 0xCC;
        cpu.X = 0x04;
        cpu.Z = true;
        cpu.N = true;
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x02;
        mem[0x0006] = 0x00; // 0x2 + 0x4
        mem[0x0007] = 0x80; 
        mem[0x8000] = 0x84;
        CPU cpuCopy = cpu;
        constexpr s32 EXPECTED_CYCLES = 6;
        // when:
        s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        Byte resultA = logicalOp(cpuCopy.A, mem[0x8000]);
        EXPECT_EQ(cpu.A, resultA);
        EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
        if (resultA == 0)
            EXPECT_TRUE(cpu.Z);
        else
            EXPECT_FALSE(cpu.Z);
        if (resultA >> 7 == 0x01)
            EXPECT_TRUE(cpu.N);
        else
            EXPECT_FALSE(cpu.N);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }

    template <typename T>
    void TestLogicalOnARegisterIndirectY(Byte opcode, T logicalOp) {
        // given:
        cpu.A = 0xCC;
        cpu.Y = 0x04;
        cpu.Z = true;
        cpu.N = true;
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x02;
        mem[0x0002] = 0x00;
        mem[0x0003] = 0x80; 
        mem[0x8004] = 0x84; // 0x8000 + 0x04(Y)
        CPU cpuCopy = cpu;
        constexpr s32 EXPECTED_CYCLES = 5;
        // when:
        s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        Byte resultA = logicalOp(cpuCopy.A, mem[0x8004]);
        EXPECT_EQ(cpu.A, resultA);
        EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
        if (resultA == 0)
            EXPECT_TRUE(cpu.Z);
        else
            EXPECT_FALSE(cpu.Z);
        if (resultA >> 7 == 0x01)
            EXPECT_TRUE(cpu.N);
        else
            EXPECT_FALSE(cpu.N);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }
    
    template <typename T>
    void TestLogicalOnARegisterIndirectYWhenCrossPage(Byte opcode, T logicalOp) {
        // given:
        cpu.A = 0xCC;
        cpu.Y = 0xFF;
        cpu.Z = true;
        cpu.N = true;
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x02;
        mem[0x0002] = 0x02;
        mem[0x0003] = 0x80; 
        mem[0x8101] = 0x84; // 0x8002 + 0xFF(Y)
        CPU cpuCopy = cpu;
        constexpr s32 EXPECTED_CYCLES = 6;
        // when:
        s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        Byte resultA = logicalOp(cpuCopy.A, mem[0x8101]);
        EXPECT_EQ(cpu.A, resultA);
        EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
        if (resultA == 0)
            EXPECT_TRUE(cpu.Z);
        else
            EXPECT_FALSE(cpu.Z);
        if (resultA >> 7 == 0x01)
            EXPECT_TRUE(cpu.N);
        else
            EXPECT_FALSE(cpu.N);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }
};

TEST_F(LogicalTests, ANDImmediate) {
    TestLogicalOnARegisterImmediate(CPU::INS_AND_IM,
        [](Byte lhs, Byte rhs) { return lhs & rhs; });
}

TEST_F(LogicalTests, EORImmediate) {
    TestLogicalOnARegisterImmediate(CPU::INS_EOR_IM,
        [](Byte lhs, Byte rhs) { return lhs ^ rhs; });
}

TEST_F(LogicalTests, ORAImmediate) {
    TestLogicalOnARegisterImmediate(CPU::INS_ORA_IM,
        [](Byte lhs, Byte rhs) { return lhs | rhs; });
}

TEST_F(LogicalTests, ANDZeroPage) {
    TestLogicalOnARegisterZeroPage(CPU::INS_AND_ZP,
        [](Byte lhs, Byte rhs) { return lhs & rhs; });
}

TEST_F(LogicalTests, EORZeroPage) {
    TestLogicalOnARegisterZeroPage(CPU::INS_EOR_ZP,
        [](Byte lhs, Byte rhs) { return lhs ^ rhs; });
}

TEST_F(LogicalTests, ORAZeroPage) {
    TestLogicalOnARegisterZeroPage(CPU::INS_ORA_ZP,
        [](Byte lhs, Byte rhs) { return lhs | rhs; });
}

TEST_F(LogicalTests, ANDZeroPageX) {
    TestLogicalOnARegisterZeroPageX(CPU::INS_AND_ZPX,
        [](Byte lhs, Byte rhs) { return lhs & rhs; });
}

TEST_F(LogicalTests, EORZeroPageX) {
    TestLogicalOnARegisterZeroPageX(CPU::INS_EOR_ZPX,
        [](Byte lhs, Byte rhs) { return lhs ^ rhs; });
}

TEST_F(LogicalTests, ORAZeroPageX) {
    TestLogicalOnARegisterZeroPageX(CPU::INS_ORA_ZPX,
        [](Byte lhs, Byte rhs) { return lhs | rhs; });
}

TEST_F(LogicalTests, ANDAbsolute) {
    TestLogicalOnARegisterAbsolute(CPU::INS_AND_ABS,
        [](Byte lhs, Byte rhs) { return lhs & rhs; });
}

TEST_F(LogicalTests, EORAbsolute) {
    TestLogicalOnARegisterAbsolute(CPU::INS_EOR_ABS,
        [](Byte lhs, Byte rhs) { return lhs ^ rhs; });
}

TEST_F(LogicalTests, ORAAbsolute) {
    TestLogicalOnARegisterAbsolute(CPU::INS_ORA_ABS,
        [](Byte lhs, Byte rhs) { return lhs | rhs; });
}

TEST_F(LogicalTests, ANDAbsoluteX) {
    TestLogicalOnARegisterAbsoluteXY(CPU::INS_AND_ABSX, &CPU::X,
        [](Byte lhs, Byte rhs) { return lhs & rhs; });
}

TEST_F(LogicalTests, EORAbsoluteX) {
    TestLogicalOnARegisterAbsoluteXY(CPU::INS_EOR_ABSX, &CPU::X,
        [](Byte lhs, Byte rhs) { return lhs ^ rhs; });
}

TEST_F(LogicalTests, ORAAbsoluteX) {
    TestLogicalOnARegisterAbsoluteXY(CPU::INS_ORA_ABSX, &CPU::X,
        [](Byte lhs, Byte rhs) { return lhs | rhs; });
}

TEST_F(LogicalTests, ANDAbsoluteY) {
    TestLogicalOnARegisterAbsoluteXY(CPU::INS_AND_ABSY, &CPU::Y,
        [](Byte lhs, Byte rhs) { return lhs & rhs; });
}

TEST_F(LogicalTests, EORAbsoluteY) {
    TestLogicalOnARegisterAbsoluteXY(CPU::INS_EOR_ABSY, &CPU::Y,
        [](Byte lhs, Byte rhs) { return lhs ^ rhs; });
}

TEST_F(LogicalTests, ORAAbsoluteY) {
    TestLogicalOnARegisterAbsoluteXY(CPU::INS_ORA_ABSY, &CPU::Y,
        [](Byte lhs, Byte rhs) { return lhs | rhs; });
}

TEST_F(LogicalTests, ANDAbsoluteXWhenCrossPage) {
    TestLogicalOnARegisterAbsoluteXYWhenCrossPage(CPU::INS_AND_ABSX, &CPU::X,
        [](Byte lhs, Byte rhs) { return lhs & rhs; });
}

TEST_F(LogicalTests, EORAbsoluteXWhenCrossPage) {
    TestLogicalOnARegisterAbsoluteXYWhenCrossPage(CPU::INS_EOR_ABSX, &CPU::X,
        [](Byte lhs, Byte rhs) { return lhs ^ rhs; });
}

TEST_F(LogicalTests, ORAAbsoluteXWhenCrossPage) {
    TestLogicalOnARegisterAbsoluteXYWhenCrossPage(CPU::INS_ORA_ABSX, &CPU::X,
        [](Byte lhs, Byte rhs) { return lhs | rhs; });
}

TEST_F(LogicalTests, ANDAbsoluteYWhenCrossPage) {
    TestLogicalOnARegisterAbsoluteXYWhenCrossPage(CPU::INS_AND_ABSY, &CPU::Y,
        [](Byte lhs, Byte rhs) { return lhs & rhs; });
}

TEST_F(LogicalTests, EORAbsoluteYWhenCrossPage) {
    TestLogicalOnARegisterAbsoluteXYWhenCrossPage(CPU::INS_EOR_ABSY, &CPU::Y,
        [](Byte lhs, Byte rhs) { return lhs ^ rhs; });
}

TEST_F(LogicalTests, ORAAbsoluteYWhenCrossPage) {
    TestLogicalOnARegisterAbsoluteXYWhenCrossPage(CPU::INS_ORA_ABSY, &CPU::Y,
        [](Byte lhs, Byte rhs) { return lhs | rhs; });
}

TEST_F(LogicalTests, ANDIndirectX) {
    TestLogicalOnARegisterIndirectX(CPU::INS_AND_INDX, 
        [](Byte lhs, Byte rhs) { return lhs & rhs; });
}

TEST_F(LogicalTests, EORIndirectX) {
    TestLogicalOnARegisterIndirectX(CPU::INS_EOR_INDX, 
        [](Byte lhs, Byte rhs) { return lhs ^ rhs; });
}

TEST_F(LogicalTests, ORAIndirectX) {
    TestLogicalOnARegisterIndirectX(CPU::INS_ORA_INDX, 
        [](Byte lhs, Byte rhs) { return lhs | rhs; });
}

TEST_F(LogicalTests, ANDIndirectY) {
    TestLogicalOnARegisterIndirectY(CPU::INS_AND_INDY, 
        [](Byte lhs, Byte rhs) { return lhs & rhs; });
}

TEST_F(LogicalTests, EORIndirectY) {
    TestLogicalOnARegisterIndirectY(CPU::INS_EOR_INDY, 
        [](Byte lhs, Byte rhs) { return lhs ^ rhs; });
}

TEST_F(LogicalTests, ORAIndirectY) {
    TestLogicalOnARegisterIndirectY(CPU::INS_ORA_INDY, 
        [](Byte lhs, Byte rhs) { return lhs | rhs; });
}

TEST_F(LogicalTests, ANDIndirectYWhenCrossPage) {
    TestLogicalOnARegisterIndirectYWhenCrossPage(CPU::INS_AND_INDY, 
        [](Byte lhs, Byte rhs) { return lhs & rhs; });
}

TEST_F(LogicalTests, EORIndirectYWhenCrossPage) {
    TestLogicalOnARegisterIndirectYWhenCrossPage(CPU::INS_EOR_INDY, 
        [](Byte lhs, Byte rhs) { return lhs ^ rhs; });
}

TEST_F(LogicalTests, ORAIndirectYWhenCrossPage) {
    TestLogicalOnARegisterIndirectYWhenCrossPage(CPU::INS_ORA_INDY, 
        [](Byte lhs, Byte rhs) { return lhs | rhs; });
}

TEST_F(LogicalTests, BITZeroPage) {
    // given:
    cpu.A = 0xFF;
    cpu.Z = true;
    cpu.N = cpu.V = false;
    mem[0xFFFC] = CPU::INS_BIT_ZP;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0b11000000;
    CPU cpuCopy = cpu;
    constexpr s32 EXPECTED_CYCLES = 3;
    // when:
    s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.A, cpuCopy.A);
    EXPECT_FALSE(cpu.Z);
    EXPECT_TRUE(cpu.N);
    EXPECT_TRUE(cpu.V);
}

TEST_F(LogicalTests, BITZeroPageRegisterNVZero) {
    // given:
    cpu.A = 0xFF;
    cpu.Z = true;
    cpu.N = cpu.V = true;
    mem[0xFFFC] = CPU::INS_BIT_ZP;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0b00111111;
    CPU cpuCopy = cpu;
    constexpr s32 EXPECTED_CYCLES = 3;
    // when:
    s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.A, cpuCopy.A);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    EXPECT_FALSE(cpu.V);
}

TEST_F(LogicalTests, BITZeroPageResultZero) {
    // given:
    cpu.A = 0xFF;
    cpu.Z = false;
    cpu.N = cpu.V = false;
    mem[0xFFFC] = CPU::INS_BIT_ZP;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x00;
    CPU cpuCopy = cpu;
    constexpr s32 EXPECTED_CYCLES = 3;
    // when:
    s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.A, cpuCopy.A);
    EXPECT_TRUE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LogicalTests, BITZeroPageResultMixed) {
    // given:
    cpu.A = 0xFF;
    cpu.Z = true;
    cpu.V = false;
    cpu.N = true;
    mem[0xFFFC] = CPU::INS_BIT_ZP;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0b01000000;
    CPU cpuCopy = cpu;
    constexpr s32 EXPECTED_CYCLES = 3;
    // when:
    s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.A, cpuCopy.A);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    EXPECT_TRUE(cpu.V);
}


TEST_F(LogicalTests, BITAbsolute) {
    // given:
    cpu.A = 0xFF;
    cpu.Z = true;
    cpu.N = cpu.V = false;
    mem[0xFFFC] = CPU::INS_BIT_ABS;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4480] = 0b11000000;
    CPU cpuCopy = cpu;
    constexpr s32 EXPECTED_CYCLES = 4;
    // when:
    s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.A, cpuCopy.A);
    EXPECT_FALSE(cpu.Z);
    EXPECT_TRUE(cpu.N);
    EXPECT_TRUE(cpu.V);
}

TEST_F(LogicalTests, BITAbsoluteRegisterNVZero) {
    // given:
    cpu.A = 0xFF;
    cpu.Z = true;
    cpu.N = cpu.V = true;
    mem[0xFFFC] = CPU::INS_BIT_ABS;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4480] = 0b00111111;
    CPU cpuCopy = cpu;
    constexpr s32 EXPECTED_CYCLES = 4;
    // when:
    s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.A, cpuCopy.A);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    EXPECT_FALSE(cpu.V);
}

TEST_F(LogicalTests, BITAbsoluteResultZero) {
    // given:
    cpu.A = 0xFF;
    cpu.Z = false;
    cpu.N = cpu.V = true;
    mem[0xFFFC] = CPU::INS_BIT_ABS;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4480] = 0b00;
    CPU cpuCopy = cpu;
    constexpr s32 EXPECTED_CYCLES = 4;
    // when:
    s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.A, cpuCopy.A);
    EXPECT_TRUE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    EXPECT_FALSE(cpu.V);
}

TEST_F(LogicalTests, BITAbsoluteResultMixed) {
    // given:
    cpu.A = 0xFF;
    cpu.Z = true;
    cpu.V = true;
    cpu.N = false;
    mem[0xFFFC] = CPU::INS_BIT_ABS;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4480] = 0b10000000;
    CPU cpuCopy = cpu;
    constexpr s32 EXPECTED_CYCLES = 4;
    // when:
    s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.A, cpuCopy.A);
    EXPECT_FALSE(cpu.Z);
    EXPECT_TRUE(cpu.N);
    EXPECT_FALSE(cpu.V);
}

