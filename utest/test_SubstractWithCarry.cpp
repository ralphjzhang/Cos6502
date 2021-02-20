#include <gtest/gtest.h>

#include "../core/cp6502.hpp"

using namespace cp6502;

struct SubstractWithCarryTests : public testing::Test {
    Mem mem;
    CPU cpu;

    virtual void SetUp() {
        cpu.Reset(0xFF00, mem);
    }

    virtual void TearDown() {
    }

    void ExpectUnaffectedRegisters(CPU const& cpuCopy) {
        EXPECT_EQ(cpu.I, cpuCopy.I);
        EXPECT_EQ(cpu.D, cpuCopy.D);
        EXPECT_EQ(cpu.B, cpuCopy.B);
    }

    struct SBCTestData {
        bool Carry;
        Byte A;
        Byte Operand;

        Byte Answer;
        bool ExpectC;
        bool ExpectZ;
        bool ExpectV;
        bool ExpectN;
    };

    SBCTestData data_20_17_WithNoCarry = {
        .Carry = true,
        .A = 20,
        .Operand = 17,

        .Answer = 3,
        .ExpectC = true,
        .ExpectZ = false,
        .ExpectV = false,
        .ExpectN = false,
    };

    SBCTestData data_n20_n17_WithNoCarry = {
        .Carry = true,
        .A = Byte(-20),
        .Operand = Byte(-17),

        .Answer = Byte(-3),
        .ExpectC = false,
        .ExpectZ = false,
        .ExpectV = false,
        .ExpectN = true,
    };


    template <typename SetMemReg>
    void Test(SBCTestData t, SetMemReg setMemReg) {
        // given:
        cpu.C = t.Carry;
        cpu.A = t.A;
        cpu.Z = !t.ExpectZ;
        cpu.V = !t.ExpectV;
        cpu.N = !t.ExpectN;
        const s32 EXPECTED_CYCELS = setMemReg(t.Operand);
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCELS, mem);
        // then:
        EXPECT_EQ(actualCycles, EXPECTED_CYCELS);
        EXPECT_EQ(cpu.A, t.Answer);
        EXPECT_EQ(cpu.C, t.ExpectC);
        EXPECT_EQ(cpu.Z, t.ExpectZ);
        EXPECT_EQ(cpu.V, t.ExpectV);
        EXPECT_EQ(cpu.N, t.ExpectN);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    void TestAbs(SBCTestData t) {
        auto setAbsMem = [this](Byte operand) {
            mem[0xFF00] = CPU::INS_SBC_ABS;
            mem[0xFF01] = 0x00;
            mem[0xFF02] = 0x80;
            mem[0x8000] = operand;
            return 4;
        };
        Test(t, setAbsMem);
    }
};

TEST_F(SubstractWithCarryTests, SBCAbsSub_0_0_WithNoCarry) {
    SBCTestData test = {
        .Carry = true,
        .A = 0,
        .Operand = 0,

        .Answer = 0,
        .ExpectC = true,
        .ExpectZ = true,
        .ExpectV = false,
        .ExpectN = false,
    };
    TestAbs(test);
}

TEST_F(SubstractWithCarryTests, SBCAbsSub_0_0_WithCarry) {
    SBCTestData test = {
        .Carry = false,
        .A = 0,
        .Operand = 0,

        .Answer = Byte(-1),
        .ExpectC = false,
        .ExpectZ = false,
        .ExpectV = false,
        .ExpectN = true,
    };
    TestAbs(test);
}

TEST_F(SubstractWithCarryTests, SBCAbsSub_0_1_WithNoCarry) {
    SBCTestData test = {
        .Carry = true,
        .A = 0,
        .Operand = 1,

        .Answer = (Byte)-1,
        .ExpectC = false,
        .ExpectZ = false,
        .ExpectV = false,
        .ExpectN = true,
    };
    TestAbs(test);
}

TEST_F(SubstractWithCarryTests, SBCAbsSub_0_1_WithCarry) {
    SBCTestData test = {
        .Carry = false,
        .A = 0,
        .Operand = 1,

        .Answer = (Byte)-2,
        .ExpectC = false,
        .ExpectZ = false,
        .ExpectV = false,
        .ExpectN = true,
    };
    TestAbs(test);
}

TEST_F(SubstractWithCarryTests, SBCAbsSub_n128_1_WithNoCarry) {
    SBCTestData test = {
        .Carry = true,
        .A = Byte(-128),
        .Operand = 1,

        .Answer = 127,
        .ExpectC = true,
        .ExpectZ = false,
        .ExpectV = true,
        .ExpectN = false,
    };
    TestAbs(test);
}

TEST_F(SubstractWithCarryTests, SBCAbsSub_127_n1_WithNoCarry) {
    SBCTestData test = {
        .Carry = true,
        .A = 127,
        .Operand = Byte(-1),

        .Answer = Byte(-128),
        .ExpectC = false,
        .ExpectZ = false,
        .ExpectV = true,
        .ExpectN = true,
    };
    TestAbs(test);
}

TEST_F(SubstractWithCarryTests, SBCAbsSub_20_17_WithNoCarry) {
    TestAbs(data_20_17_WithNoCarry);
}

TEST_F(SubstractWithCarryTests, SBCAbsSub_n20_n17_WithNoCarry) {
    TestAbs(data_n20_n17_WithNoCarry);
}
