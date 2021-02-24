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

    template <typename SetRegister>
    void BranchForward(Byte opcode, SetRegister setRegister) {
        // given:
        setRegister();
        mem[0xFF00] = opcode;
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

    template <typename SetRegister>
    void BranchBackword(Byte opcode, SetRegister setRegister) {
        // given:
        cpu.Reset(0xFFCC, mem);
        setRegister();
        signed char offset = -2;
        mem[0xFFCC] = opcode;
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

    template <typename SetRegister>
    void DoNotBranch(Byte opcode, SetRegister setRegister) {
        // given:
        setRegister();
        mem[0xFF00] = opcode;
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

    template <typename SetRegister>
    void BranchCrossPage(Byte opcode, SetRegister setRegister) {
        // given:
        cpu.Reset(0xFEFD, mem);
        setRegister();
        mem[0xFEFD] = opcode;
        mem[0xFEFE] = 0x1;
        constexpr s32 EXPECTED_CYCLES = 4;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
        EXPECT_EQ(cpu.PC, 0xFF00);
        EXPECT_EQ(cpu.PS, cpuCopy.PS);
    }

    template <typename SetRegister>
    void BranchBackwordsCrossPage(Byte opcode, SetRegister setRegister) {
        // given:
        cpu.Reset(0xFF00, mem);
        setRegister();
        signed char offset = -3;
        mem[0xFF00] = opcode;
        mem[0xFF01] = offset; // branch to 0xFEFF
        constexpr s32 EXPECTED_CYCLES = 4;
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
        EXPECT_EQ(cpu.PC, 0xFEFF);
        EXPECT_EQ(cpu.PS, cpuCopy.PS);
    }

    void BranchInProgram(Byte opcode, Byte z) {
        // given:
        cpu.Reset(0xFF00, mem);
        cpu.Z = z;
        /*
            loop
            lda #0
            beq loop
        */
        mem[0xFF00] = 0xA9;
        mem[0xFF01] = 0x00;
        mem[0xFF02] = opcode;
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
};

TEST_F(BranchesTests, BEQBranchesWhenZIsSet) {
    BranchForward(CPU::INS_BEQ, [this]() { cpu.Z = true; });
}
TEST_F(BranchesTests, BNEBranchesWhenZIsClear) {
    BranchForward(CPU::INS_BNE, [this]() { cpu.Z = false; });
}
TEST_F(BranchesTests, BCSBranchesWhenCIsSet) {
    BranchForward(CPU::INS_BCS, [this]() { cpu.C = true; });
}
TEST_F(BranchesTests, BCCBranchesWhenCIsClear) {
    BranchForward(CPU::INS_BCC, [this]() { cpu.C = false; });
}
TEST_F(BranchesTests, BMIBranchesWhenNIsSet) {
    BranchForward(CPU::INS_BMI, [this]() { cpu.N = true; });
}
TEST_F(BranchesTests, BPLBranchesWhenNIsClear) {
    BranchForward(CPU::INS_BPL, [this]() { cpu.N = false; });
}
TEST_F(BranchesTests, BVSBranchesWhenVIsSet) {
    BranchForward(CPU::INS_BVS, [this]() { cpu.V = true; });
}
TEST_F(BranchesTests, BVCBranchesWhenVIsClear) {
    BranchForward(CPU::INS_BVC, [this]() { cpu.V = false; });
}


TEST_F(BranchesTests, BEQBranchesBackwordsWhenZIsSet) {
    BranchBackword(CPU::INS_BEQ, [this]() { cpu.Z = true; });
}
TEST_F(BranchesTests, BNEBranchesBackwordsWhenZIsClear) {
    BranchBackword(CPU::INS_BNE, [this]() { cpu.Z = false; });
}
TEST_F(BranchesTests, BCSBranchesBackwordsWhenCIsSet) {
    BranchBackword(CPU::INS_BCS, [this]() { cpu.C = true; });
}
TEST_F(BranchesTests, BCCBranchesBackwordsWhenCIsClear) {
    BranchBackword(CPU::INS_BCC, [this]() { cpu.C = false; });
}
TEST_F(BranchesTests, BMIBranchesBackwordsWhenNIsSet) {
    BranchBackword(CPU::INS_BMI, [this]() { cpu.N = true; });
}
TEST_F(BranchesTests, BPLBranchesBackwordsWhenNIsClear) {
    BranchBackword(CPU::INS_BPL, [this]() { cpu.N = false; });
}
TEST_F(BranchesTests, BVSBranchesBackwordsWhenVIsSet) {
    BranchBackword(CPU::INS_BVS, [this]() { cpu.V = true; });
}
TEST_F(BranchesTests, BVCBranchesBackwordsWhenVIsClear) {
    BranchBackword(CPU::INS_BVC, [this]() { cpu.N = false; });
}


TEST_F(BranchesTests, BEQDoesNotBranchWhenZIsClear) {
    DoNotBranch(CPU::INS_BEQ, [this]() { cpu.Z = false; });
}
TEST_F(BranchesTests, BNEDoesNotBranchWhenZIsSet) {
    DoNotBranch(CPU::INS_BNE, [this]() { cpu.Z = true; });
}
TEST_F(BranchesTests, BCSDoesNotBranchWhenCIsClear) {
    DoNotBranch(CPU::INS_BCS, [this]() { cpu.C = false; });
}
TEST_F(BranchesTests, BCCDoesNotBranchWhenCIsSet) {
    DoNotBranch(CPU::INS_BCC, [this]() { cpu.C = true; });
}
TEST_F(BranchesTests, BMIDoesNotBranchWhenNIsClear) {
    DoNotBranch(CPU::INS_BMI, [this]() { cpu.N = false; });
}
TEST_F(BranchesTests, BPLDoesNotBranchWhenNIsSet) {
    DoNotBranch(CPU::INS_BPL, [this]() { cpu.N = true; });
}
TEST_F(BranchesTests, BVSDoesNotBranchWhenVIsClear) {
    DoNotBranch(CPU::INS_BVS, [this]() { cpu.V = false; });
}
TEST_F(BranchesTests, BVCDoesNotBranchWhenVIsSet) {
    DoNotBranch(CPU::INS_BVC, [this]() { cpu.V = true; });
}


TEST_F(BranchesTests, BEQBranchesCrossPage) {
    BranchCrossPage(CPU::INS_BEQ, [this]() { cpu.Z = true; });
}
TEST_F(BranchesTests, BNEBranchesCrossPage) {
    BranchCrossPage(CPU::INS_BNE, [this]() { cpu.Z = false; });
}
TEST_F(BranchesTests, BCSBranchesCrossPage) {
    BranchCrossPage(CPU::INS_BCS, [this]() { cpu.C = true; });
}
TEST_F(BranchesTests, BCCBranchesCrossPage) {
    BranchCrossPage(CPU::INS_BCC, [this]() { cpu.C = false; });
}
TEST_F(BranchesTests, BMIBranchesCrossPage) {
    BranchCrossPage(CPU::INS_BMI, [this]() { cpu.N = true; });
}
TEST_F(BranchesTests, BPLBranchesCrossPage) {
    BranchCrossPage(CPU::INS_BPL, [this]() { cpu.N = false; });
}
TEST_F(BranchesTests, BVSBranchesCrossPage) {
    BranchCrossPage(CPU::INS_BVS, [this]() { cpu.V = true; });
}
TEST_F(BranchesTests, BVCBranchesCrossPage) {
    BranchCrossPage(CPU::INS_BVC, [this]() { cpu.V = false; });
}


TEST_F(BranchesTests, BEQBranchesBackwordsCrossPage) {
    BranchBackwordsCrossPage(CPU::INS_BEQ, [this]() { cpu.Z = true; });
}
TEST_F(BranchesTests, BNEBranchesBackwordsCrossPage) {
    BranchBackwordsCrossPage(CPU::INS_BNE, [this]() { cpu.Z = false; });
}
TEST_F(BranchesTests, BCSBranchesBackwordsCrossPage) {
    BranchBackwordsCrossPage(CPU::INS_BCS, [this]() { cpu.C = true; });
}
TEST_F(BranchesTests, BCCBranchesBackwordsCrossPage) {
    BranchBackwordsCrossPage(CPU::INS_BCC, [this]() { cpu.C = false; });
}
TEST_F(BranchesTests, BMIBranchesBackwordsCrossPage) {
    BranchBackwordsCrossPage(CPU::INS_BMI, [this]() { cpu.N = true; });
}
TEST_F(BranchesTests, BPLBranchesBackwordsCrossPage) {
    BranchBackwordsCrossPage(CPU::INS_BPL, [this]() { cpu.N = false; });
}
TEST_F(BranchesTests, BVSBranchesBackwordsCrossPage) {
    BranchBackwordsCrossPage(CPU::INS_BVS, [this]() { cpu.V = true; });
}
TEST_F(BranchesTests, BVCBranchesBackwordsCrossPage) {
    BranchBackwordsCrossPage(CPU::INS_BVC, [this]() { cpu.V = false; });
}


TEST_F(BranchesTests, BEQInProgram) {
    // given:
    cpu.Reset(0xFF00, mem);
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
}
