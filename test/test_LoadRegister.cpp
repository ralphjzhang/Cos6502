#include <gtest/gtest.h>

#include "../core/cos6502.hpp"

using namespace cos6502;

struct LoadRegisterTests : public testing::Test {
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

    void TestLoadImmediateRegister(Byte opcode, Byte CPU::*Register) {
        // given:
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x84;
        CPU cpuCopy = cpu;
        // when:
        s32 cycleUsed = cpu.Execute(2, mem);
        // then:
        EXPECT_EQ(cpu.*Register, 0x84);
        EXPECT_EQ(cycleUsed, 2);
        EXPECT_FALSE(cpu.Z);
        EXPECT_TRUE(cpu.N);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }

    void TestLoadRegisterZeroPage(Byte opcode, Byte CPU::*Register) {
        // given:
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x42;
        mem[0x0042] = 0x37;
        CPU cpuCopy = cpu;
        // when:
        s32 cycleUsed = cpu.Execute(3, mem);
        // then:
        EXPECT_EQ(cpu.*Register, 0x37);
        EXPECT_EQ(cycleUsed, 3);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }

    void TestLoadRegisterZeroPageXY(Byte opcode, Byte CPU::*RegisterToAdd, Byte CPU::*Register) {
        // given:
        cpu.*RegisterToAdd = 5;
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x42;
        mem[0x0047] = 0x37;
        CPU cpuCopy = cpu;
        // when:
        s32 cycleUsed = cpu.Execute(4, mem);
        // then:
        EXPECT_EQ(cpu.*Register, 0x37);
        EXPECT_EQ(cycleUsed, 4);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }

   void TestLoadAbsolute(Byte opcode, Byte CPU::*Register) {
        // given:
        cpu.Z = cpu.N = true;
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x80;
        mem[0xFFFE] = 0x44; // 0x4480
        mem[0x4480] = 0x37;
        constexpr s32 EXPECTED_CYCLES = 4;
        CPU cpuCopy = cpu;
        // when:
        s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(cpu.*Register, 0x37);
        EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }

    void TestLoadAbsoluteXY(Byte opcode, Byte CPU::*RegisterToAdd, Byte CPU::*Register) {
        // given:
        cpu.Z = cpu.N = true;
        cpu.*RegisterToAdd = 1;
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x80;
        mem[0xFFFE] = 0x44; // 0x4480
        mem[0x4481] = 0x37; 
        constexpr s32 EXPECTED_CYCLES = 4;
        CPU cpuCopy = cpu;
        // when:
        s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(cpu.*Register, 0x37);
        EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }

    void TestLoadAbsoluteXYWhenCrossPage(Byte opcode, Byte CPU::*RegisterToAdd, Byte CPU::*Register) {
        // given:
        cpu.*RegisterToAdd = 0xFF;
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x02;
        mem[0xFFFE] = 0x44; // 0x4402
        mem[0x4501] = 0x37; // 0x4402+0x00FF, 
        constexpr s32 EXPECTED_CYCLES = 5;
        CPU cpuCopy = cpu;
        // when:
        s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(cpu.*Register, 0x37);
        EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
        VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
    }
};

TEST_F(LoadRegisterTests, CPUDoesNothingWhenExecuteZeroCycles) {
    // given:
    constexpr s32 NUM_CYCLES = 0;
    // when:
    s32 cyclesUsed = cpu.Execute(NUM_CYCLES, mem);
    // then:
    EXPECT_EQ(cyclesUsed, 0);
}

TEST_F(LoadRegisterTests, CPUExecutesMoreCyclesThanRequestedWhenInstructionNeeded) {
    // given:
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x84;
    // when:
    s32 cycleUsed = cpu.Execute(1, mem);
    // then:
    EXPECT_EQ(cycleUsed, 2);
}

TEST_F(LoadRegisterTests, LDAImmediateCanLoadAValueIntoTheARegister) {
    TestLoadImmediateRegister(CPU::INS_LDA_IM, &CPU::A);
}

TEST_F(LoadRegisterTests, LDXImmediateCanLoadAValueIntoTheXRegister) {
    TestLoadImmediateRegister(CPU::INS_LDX_IM, &CPU::X);
}

TEST_F(LoadRegisterTests, LDYImmediateCanLoadAValueIntoTheYRegister) {
    TestLoadImmediateRegister(CPU::INS_LDY_IM, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDAImmediateCanAffectTheZeroFlag) {
    // given:
    cpu.A = 0x44;
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x0;
    CPU cpuCopy = cpu;
    // when:
    s32 cycleUsed = cpu.Execute(2, mem);
    // then:
    EXPECT_TRUE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
}


TEST_F(LoadRegisterTests, LDAZeroPageCanLoadAValueIntoTheARegister) {
    TestLoadRegisterZeroPage(CPU::INS_LDA_ZP, &CPU::A);
}

TEST_F(LoadRegisterTests, LDXZeroPageCanLoadAValueIntoTheXRegister) {
    TestLoadRegisterZeroPage(CPU::INS_LDX_ZP, &CPU::X);
}

TEST_F(LoadRegisterTests, LDYZeroPageCanLoadAValueIntoTheYRegister) {
    TestLoadRegisterZeroPage(CPU::INS_LDY_ZP, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDAZeroPageXCanLoadAValueIntoTheARegister) {
    TestLoadRegisterZeroPageXY(CPU::INS_LDA_ZPX, &CPU::X, &CPU::A);
}

TEST_F(LoadRegisterTests, LDXZeroPageYCanLoadAValueIntoTheXRegister) {
    TestLoadRegisterZeroPageXY(CPU::INS_LDX_ZPY, &CPU::Y, &CPU::X);
}

TEST_F(LoadRegisterTests, LDYZeroPageXCanLoadAValueIntoTheYRegister) {
    TestLoadRegisterZeroPageXY(CPU::INS_LDY_ZPX, &CPU::X, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDAZeroPageXCanLoadAValueIntoTheARegisterWhenItWraps) {
    // given:
    cpu.X = 0xFF;
    mem[0xFFFC] = CPU::INS_LDA_ZPX;
    mem[0xFFFD] = 0x80;
    mem[0x007F] = 0x37;
    CPU cpuCopy = cpu;
    // when:
    s32 cycleUsed = cpu.Execute(4, mem);
    // then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cycleUsed, 4);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
}

TEST_F(LoadRegisterTests, LDAAbsoluteCanLoadAValueIntoTheARegister) {
    TestLoadAbsolute(CPU::INS_LDA_ABS, &CPU::A);
}

TEST_F(LoadRegisterTests, LDXAbsoluteCanLoadAValueIntoTheXRegister) {
    TestLoadAbsolute(CPU::INS_LDX_ABS, &CPU::X);
}

TEST_F(LoadRegisterTests, LDYAbsoluteCanLoadAValueIntoTheYRegister) {
    TestLoadAbsolute(CPU::INS_LDY_ABS, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDAAbsoluteXCanLoadAValueIntoTheARegister) {
    TestLoadAbsoluteXY(CPU::INS_LDA_ABSX, &CPU::X, &CPU::A);
}

TEST_F(LoadRegisterTests, LDAAbsoluteYCanLoadAValueIntoTheARegister) {
    TestLoadAbsoluteXY(CPU::INS_LDA_ABSY, &CPU::Y, &CPU::A);
}

TEST_F(LoadRegisterTests, LDXAbsoluteYCanLoadAValueIntoTheXRegister) {
    TestLoadAbsoluteXY(CPU::INS_LDX_ABSY, &CPU::Y, &CPU::X);
}

TEST_F(LoadRegisterTests, LDYAbsoluteXCanLoadAValueIntoTheYRegister) {
    TestLoadAbsoluteXY(CPU::INS_LDY_ABSX, &CPU::X, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDAAbsoluteXCanLoadAValueIntoTheARegisterWhenCrossPage) {
    TestLoadAbsoluteXYWhenCrossPage(CPU::INS_LDA_ABSX, &CPU::X, &CPU::A);
}

TEST_F(LoadRegisterTests, LDAAbsoluteYCanLoadAValueIntoTheARegisterWhenCrossPage) {
    TestLoadAbsoluteXYWhenCrossPage(CPU::INS_LDA_ABSY, &CPU::Y, &CPU::A);
}

TEST_F(LoadRegisterTests, LDXAbsoluteYCanLoadAValueIntoTheXRegisterWhenCrossPage) {
    TestLoadAbsoluteXYWhenCrossPage(CPU::INS_LDX_ABSY, &CPU::Y, &CPU::X);
}

TEST_F(LoadRegisterTests, LDYAbsoluteXCanLoadAValueIntoTheYRegisterWhenCrossPage) {
    TestLoadAbsoluteXYWhenCrossPage(CPU::INS_LDY_ABSX, &CPU::X, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDAIndirectXCanLoadAValueIntoTheARegister) {
    // given:
    cpu.Z = cpu.N = true;
    cpu.X = 0x04;
    mem[0xFFFC] = CPU::INS_LDA_INDX;
    mem[0xFFFD] = 0x02;
    mem[0x0006] = 0x00; // 0x2 + 0x4
    mem[0x0007] = 0x80; 
    mem[0x8000] = 0x37;
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU cpuCopy = cpu;
    // when:
    s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
}

TEST_F(LoadRegisterTests, LDAIndirectYCanLoadAValueIntoTheARegister) {
    // given:
    cpu.Z = cpu.N = true;
    cpu.Y = 0x04;
    mem[0xFFFC] = CPU::INS_LDA_INDY;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x00;
    mem[0x0003] = 0x80; 
    mem[0x8004] = 0x37; // 0x8000 + 0x04(Y)
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU cpuCopy = cpu;
    // when:
    s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
}

TEST_F(LoadRegisterTests, LDAIndirectYCanLoadAValueIntoTheARegisterWhenCrossAPage) {
    // given:
    cpu.Y = 0xFF;
    mem[0xFFFC] = CPU::INS_LDA_INDY;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x02;
    mem[0x0003] = 0x80; 
    mem[0x8101] = 0x37; // 0x8002 + 0xFF(Y)
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU cpuCopy = cpu;
    // when:
    s32 cycleUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cycleUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, cpuCopy);
}

