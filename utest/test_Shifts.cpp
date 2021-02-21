#include <gtest/gtest.h>

#include "../core/cp6502.hpp"

using namespace cp6502;

struct ShiftTests : public testing::Test {
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
        EXPECT_EQ(cpu.V, cpuCopy.V);
    }

    struct ShiftData {
        Byte Operand;

        Byte Answer;
        bool ExpectC;
        bool ExpectZ;
        bool ExpectN;
    };

    template <typename SetMemReg>
    void Test(ShiftData t, SetMemReg setMemReg, Byte* result) {
        // given:
        cpu.C = !t.ExpectC;
        cpu.Z = !t.ExpectZ;
        cpu.N = !t.ExpectN;
        cpu.A = ~t.Answer;
        const s32 EXPECTED_CYCLES = setMemReg(t.Operand);
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
        EXPECT_EQ(*result, t.Answer);
        EXPECT_EQ(cpu.C, t.ExpectC);
        EXPECT_EQ(cpu.Z, t.ExpectZ);
        EXPECT_EQ(cpu.N, t.ExpectN);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    ShiftData asl_0 = {
        .Operand = 0,
        .Answer = 0,
        .ExpectC = 0,
        .ExpectZ = true,
        .ExpectN = false
    };

    ShiftData asl_neg = {
        .Operand = 0b11000010,
        .Answer = 0b10000100,
        .ExpectC = 1,
        .ExpectZ = false,
        .ExpectN = true
    };

    ShiftData asl_pos = {
        .Operand = 0b00000010,
        .Answer = 0b00000100,
        .ExpectC = 0,
        .ExpectZ = false,
        .ExpectN = false
    };

    void TestASL_ACC(ShiftData t) {
        auto set = [this](Byte operand) {
            mem[0xFF00] = CPU::INS_ASL_ACC;
            cpu.A = operand;
            return 2;
        };
        Test(t, set, &cpu.A);
    }
    void TestASL_ZP(ShiftData t) {
        auto set = [this](Byte operand) {
            mem[0xFF00] = CPU::INS_ASL_ZP;
            mem[0xFF01] = 0x42;
            mem[0x0042] = operand;
            return 5;
        };
        Test(t, set, &mem[0x0042]);
    }
    void TestASL_ZPX(ShiftData t) {
        auto set = [this](Byte operand) {
            cpu.X = 0x01;
            mem[0xFF00] = CPU::INS_ASL_ZPX;
            mem[0xFF01] = 0x42;
            mem[0x0043] = operand;
            return 6;
        };
        Test(t, set, &mem[0x0043]);
    }
    void TestASL_ABS(ShiftData t) {
        auto set = [this](Byte operand) {
            mem[0xFF00] = CPU::INS_ASL_ABS;
            mem[0xFF01] = 0x00;
            mem[0xFF02] = 0x80;
            mem[0x8000] = operand;
            return 6;
        };
        Test(t, set, &mem[0x8000]);
    }
    void TestASL_ABSX(ShiftData t) {
        auto set = [this](Byte operand) {
            cpu.X = 0x01;
            mem[0xFF00] = CPU::INS_ASL_ABSX;
            mem[0xFF01] = 0x00;
            mem[0xFF02] = 0x80;
            mem[0x8001] = operand;
            return 7;
        };
        Test(t, set, &mem[0x8001]);
    }

    ShiftData lsr_0 = {
        .Operand = 0,
        .Answer = 0,
        .ExpectC = 0,
        .ExpectZ = true,
        .ExpectN = false
    };

    ShiftData lsr_neg = {
        .Operand = 0b11000010,
        .Answer = 0b01100001,
        .ExpectC = 0,
        .ExpectZ = false,
        .ExpectN = false
    };

    ShiftData lsr_pos = {
        .Operand = 0b00000011,
        .Answer = 0b00000001,
        .ExpectC = 1,
        .ExpectZ = false,
        .ExpectN = false
    };
    void TestLSR_ACC(ShiftData t) {
        auto set = [this](Byte operand) {
            mem[0xFF00] = CPU::INS_LSR_ACC;
            cpu.A = operand;
            return 2;
        };
        Test(t, set, &cpu.A);
    }
    void TestLSR_ZP(ShiftData t) {
        auto set = [this](Byte operand) {
            mem[0xFF00] = CPU::INS_LSR_ZP;
            mem[0xFF01] = 0x42;
            mem[0x0042] = operand;
            return 5;
        };
        Test(t, set, &mem[0x0042]);
    }
    void TestLSR_ZPX(ShiftData t) {
        auto set = [this](Byte operand) {
            cpu.X = 0x01;
            mem[0xFF00] = CPU::INS_LSR_ZPX;
            mem[0xFF01] = 0x42;
            mem[0x0043] = operand;
            return 6;
        };
        Test(t, set, &mem[0x0043]);
    }
    void TestLSR_ABS(ShiftData t) {
        auto set = [this](Byte operand) {
            mem[0xFF00] = CPU::INS_LSR_ABS;
            mem[0xFF01] = 0x00;
            mem[0xFF02] = 0x80;
            mem[0x8000] = operand;
            return 6;
        };
        Test(t, set, &mem[0x8000]);
    }
    void TestLSR_ABSX(ShiftData t) {
        auto set = [this](Byte operand) {
            cpu.X = 0x01;
            mem[0xFF00] = CPU::INS_LSR_ABSX;
            mem[0xFF01] = 0x00;
            mem[0xFF02] = 0x80;
            mem[0x8001] = operand;
            return 7;
        };
        Test(t, set, &mem[0x8001]);
    }
};

TEST_F(ShiftTests, ASL_ACC_0) {
    TestASL_ACC(asl_0);
}
TEST_F(ShiftTests, ASL_ACC_neg) {
    TestASL_ACC(asl_neg);
}
TEST_F(ShiftTests, ASL_ACC_pos) {
    TestASL_ACC(asl_pos);
}

TEST_F(ShiftTests, ASL_ZP_0) {
    TestASL_ZP(asl_0);
}
TEST_F(ShiftTests, ASL_ZP_neg) {
    TestASL_ZP(asl_neg);
}
TEST_F(ShiftTests, ASL_ZP_pos) {
    TestASL_ZP(asl_pos);
}

TEST_F(ShiftTests, ASL_ZPX_0) {
    TestASL_ZPX(asl_0);
}
TEST_F(ShiftTests, ASL_ZPX_neg) {
    TestASL_ZPX(asl_neg);
}
TEST_F(ShiftTests, ASL_ZPX_pos) {
    TestASL_ZPX(asl_pos);
}

TEST_F(ShiftTests, ASL_ABS_0) {
    TestASL_ABS(asl_0);
}
TEST_F(ShiftTests, ASL_ABS_neg) {
    TestASL_ABS(asl_neg);
}
TEST_F(ShiftTests, ASL_ABS_pos) {
    TestASL_ABS(asl_pos);
}

TEST_F(ShiftTests, ASL_ABSX_0) {
    TestASL_ABSX(asl_0);
}
TEST_F(ShiftTests, ASL_ABSX_neg) {
    TestASL_ABSX(asl_neg);
}
TEST_F(ShiftTests, ASL_ABSX_pos) {
    TestASL_ABSX(asl_pos);
}

// LSR
TEST_F(ShiftTests, LSR_ACC_0) {
    TestLSR_ACC(lsr_0);
}
TEST_F(ShiftTests, LSR_ACC_neg) {
    TestLSR_ACC(lsr_neg);
}
TEST_F(ShiftTests, LSR_ACC_pos) {
    TestLSR_ACC(lsr_pos);
}

TEST_F(ShiftTests, LSR_ZP_0) {
    TestLSR_ZP(lsr_0);
}
TEST_F(ShiftTests, LSR_ZP_neg) {
    TestLSR_ZP(lsr_neg);
}
TEST_F(ShiftTests, LSR_ZP_pos) {
    TestLSR_ZP(lsr_pos);
}

TEST_F(ShiftTests, LSR_ZPX_0) {
    TestLSR_ZPX(lsr_0);
}
TEST_F(ShiftTests, LSR_ZPX_neg) {
    TestLSR_ZPX(lsr_neg);
}
TEST_F(ShiftTests, LSR_ZPX_pos) {
    TestLSR_ZPX(lsr_pos);
}

TEST_F(ShiftTests, LSR_ABS_0) {
    TestLSR_ABS(lsr_0);
}
TEST_F(ShiftTests, LSR_ABS_neg) {
    TestLSR_ABS(lsr_neg);
}
TEST_F(ShiftTests, LSR_ABS_pos) {
    TestLSR_ABS(lsr_pos);
}

TEST_F(ShiftTests, LSR_ABSX_0) {
    TestLSR_ABSX(lsr_0);
}
TEST_F(ShiftTests, LSR_ABSX_neg) {
    TestLSR_ABSX(lsr_neg);
}
TEST_F(ShiftTests, LSR_ABSX_pos) {
    TestLSR_ABSX(lsr_pos);
}


