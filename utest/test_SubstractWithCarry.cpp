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

    void TestIM(SBCTestData t) {
        auto setImMem = [this](Byte operand) {
            mem[0xFF00] = CPU::INS_SBC_IM;
            mem[0xFF01] = operand;
            return 2; // expected cycles;
        };
        Test(t, setImMem);
    }

    void TestZP(SBCTestData t) {
        auto setZPMem = [this](Byte operand) {
            mem[0xFF00] = CPU::INS_SBC_ZP;
            mem[0xFF01] = 0x42;
            mem[0x0042] = operand;
            return 3; // expected cycles;
        };
        Test(t, setZPMem);
    }

    void TestZPX(SBCTestData t) {
        auto setZPXMem = [this](Byte operand) {
            cpu.X = 0x01;
            mem[0xFF00] = CPU::INS_SBC_ZPX;
            mem[0xFF01] = 0x42;
            mem[0x0043] = operand;
            return 4; // expected cycles;
        };
        Test(t, setZPXMem);
    }

    void TestABSX(SBCTestData t) {
        auto setABSXMem = [this](Byte operand) {
            cpu.X = 0x01;
            mem[0xFF00] = CPU::INS_SBC_ABSX;
            mem[0xFF01] = 0x00;
            mem[0xFF02] = 0x80;
            mem[0x8001] = operand;
            return 4; // expected cycles;
        };
        Test(t, setABSXMem);
    }

    void TestABSY(SBCTestData t) {
        auto setABSYMem = [this](Byte operand) {
            cpu.Y = 0x01;
            mem[0xFF00] = CPU::INS_SBC_ABSY;
            mem[0xFF01] = 0x00;
            mem[0xFF02] = 0x80;
            mem[0x8001] = operand;
            return 4; // expected cycles;
        };
        Test(t, setABSYMem);
    }

    void TestINDX(SBCTestData t) {
        auto setINDXMem = [this](Byte operand) {
            cpu.X = 0x04;
            mem[0xFF00] = CPU::INS_SBC_INDX;
            mem[0xFF01] = 0x02;
            mem[0x0006] = 0x00; // 0x2 + 0x4
            mem[0x0007] = 0x80; 
            mem[0x8000] = operand;
            return 6; // expected cycles;
        };
        Test(t, setINDXMem);
    }

    void TestINDY(SBCTestData t) {
        auto setINDYMem = [this](Byte operand) {
            cpu.Y = 0x04;
            mem[0xFF00] = CPU::INS_SBC_INDY;
            mem[0xFF01] = 0x02;
            mem[0x0002] = 0x00;
            mem[0x0003] = 0x80; 
            mem[0x8004] = operand; // 0x8000 + 0x04(Y)
            return 5; // expected cycles;
        };
        Test(t, setINDYMem);
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

TEST_F(SubstractWithCarryTests, SBCImSub_20_17_WithNoCarry) {
    TestIM(data_20_17_WithNoCarry);
}
TEST_F(SubstractWithCarryTests, SBCImSub_n20_n17_WithNoCarry) {
    TestIM(data_n20_n17_WithNoCarry);
}

TEST_F(SubstractWithCarryTests, SBCZPSub_20_17_WithNoCarry) {
    TestZP(data_20_17_WithNoCarry);
}
TEST_F(SubstractWithCarryTests, SBCZPSub_n20_n17_WithNoCarry) {
    TestZP(data_n20_n17_WithNoCarry);
}

TEST_F(SubstractWithCarryTests, SBCZPXSub_20_17_WithNoCarry) {
    TestZPX(data_20_17_WithNoCarry);
}
TEST_F(SubstractWithCarryTests, SBCZPXSub_n20_n17_WithNoCarry) {
    TestZPX(data_n20_n17_WithNoCarry);
}

TEST_F(SubstractWithCarryTests, SBCABSXSub_20_17_WithNoCarry) {
    TestABSX(data_20_17_WithNoCarry);
}
TEST_F(SubstractWithCarryTests, SBCABSXSub_n20_n17_WithNoCarry) {
    TestABSX(data_n20_n17_WithNoCarry);
}

TEST_F(SubstractWithCarryTests, SBCABSYSub_20_17_WithNoCarry) {
    TestABSY(data_20_17_WithNoCarry);
}
TEST_F(SubstractWithCarryTests, SBCABSYSub_n20_n17_WithNoCarry) {
    TestABSY(data_n20_n17_WithNoCarry);
}

TEST_F(SubstractWithCarryTests, SBCINDXSub_20_17_WithNoCarry) {
    TestINDX(data_20_17_WithNoCarry);
}
TEST_F(SubstractWithCarryTests, SBCINDXSub_n20_n17_WithNoCarry) {
    TestINDX(data_n20_n17_WithNoCarry);
}

TEST_F(SubstractWithCarryTests, SBCINDYSub_20_17_WithNoCarry) {
    TestINDY(data_20_17_WithNoCarry);
}
TEST_F(SubstractWithCarryTests, SBCINDYSub_n20_n17_WithNoCarry) {
    TestINDY(data_n20_n17_WithNoCarry);
}

