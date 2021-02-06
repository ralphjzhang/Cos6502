#include <gtest/gtest.h>

#include <core/main_6502.hpp>

class M6502Test1 : public testing::Test {
public:
    Mem mem;
    CPU cpu;

    virtual void SetUp() {
        cpu.Reset(mem);
    }

    virtual void TearDown() {
    }
};

TEST_F(M6502Test1, LDAImmediateCanLoadAValueIntoTheARegister) {
    // given:
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x84;
    CPU cpuCopy = cpu;
    // when:
    s32 cycleUsed = cpu.Execute(2, mem);
    // then:
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_EQ(cycleUsed, 2);
    EXPECT_FALSE(cpu.Z);
    EXPECT_TRUE(cpu.N);
    EXPECT_EQ(cpu.C, cpuCopy.C);
    EXPECT_EQ(cpu.I, cpuCopy.I);
    EXPECT_EQ(cpu.D, cpuCopy.D);
    EXPECT_EQ(cpu.B, cpuCopy.B);
    EXPECT_EQ(cpu.V, cpuCopy.V);
}

TEST_F(M6502Test1, LDAZeroPageCanLoadAValueIntoTheARegister) {
    // given:
    mem[0xFFFC] = CPU::INS_LDA_ZP;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x37;
    CPU cpuCopy = cpu;
    // when:
    s32 cycleUsed = cpu.Execute(3, mem);
    // then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cycleUsed, 3);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    EXPECT_EQ(cpu.C, cpuCopy.C);
    EXPECT_EQ(cpu.I, cpuCopy.I);
    EXPECT_EQ(cpu.D, cpuCopy.D);
    EXPECT_EQ(cpu.B, cpuCopy.B);
    EXPECT_EQ(cpu.V, cpuCopy.V);
}

TEST_F(M6502Test1, LDAZeroPageXCanLoadAValueIntoTheARegister) {
    // given:
    cpu.X = 5;
    mem[0xFFFC] = CPU::INS_LDA_ZPX;
    mem[0xFFFD] = 0x42;
    mem[0x0047] = 0x37;
    CPU cpuCopy = cpu;
    // when:
    s32 cycleUsed = cpu.Execute(4, mem);
    // then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cycleUsed, 4);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    EXPECT_EQ(cpu.C, cpuCopy.C);
    EXPECT_EQ(cpu.I, cpuCopy.I);
    EXPECT_EQ(cpu.D, cpuCopy.D);
    EXPECT_EQ(cpu.B, cpuCopy.B);
    EXPECT_EQ(cpu.V, cpuCopy.V);
}

TEST_F(M6502Test1, LDAZeroPageXCanLoadAValueIntoTheARegisterWhenItWraps) {
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
    EXPECT_EQ(cpu.C, cpuCopy.C);
    EXPECT_EQ(cpu.I, cpuCopy.I);
    EXPECT_EQ(cpu.D, cpuCopy.D);
    EXPECT_EQ(cpu.B, cpuCopy.B);
    EXPECT_EQ(cpu.V, cpuCopy.V);
}
