#include <gtest/gtest.h>

#include "../core/cp6502.hpp"

using namespace cp6502;

struct CompareTests : public testing::Test {
    Mem mem;
    CPU cpu;

    virtual void SetUp() {
        cpu.Reset(0xFF00, mem);
    }

    virtual void TearDown() {
    }

    struct CMPTestData {
        Byte Register;
        Byte Operand;

        bool ExpectC;
        bool ExpectZ;
        bool ExpectN;
    };

    CMPTestData data_cmp_26_26 = {
        .Register = 26,
        .Operand = 26,
        .ExpectC = true,
        .ExpectZ = true,
        .ExpectN = false
    };
    CMPTestData data_cmp_48_26 = {
        .Register = 48,
        .Operand = 26,
        .ExpectC = true,
        .ExpectZ = false,
        .ExpectN = false,
    };
    CMPTestData data_cmp_130_26 = {
        .Register = 130,   // negative
        .Operand = 26,
        .ExpectC = true,
        .ExpectZ = false,
        .ExpectN = false,
    };
    CMPTestData data_cmp_8_26 = {
        .Register = 8,
        .Operand = 26,
        .ExpectC = false,
        .ExpectZ = false,
        .ExpectN = true,
    };

    void ExpectUnaffectedRegisters(CPU const& cpuCopy) {
        EXPECT_EQ(cpu.I, cpuCopy.I);
        EXPECT_EQ(cpu.D, cpuCopy.D);
        EXPECT_EQ(cpu.B, cpuCopy.B);
        EXPECT_EQ(cpu.V, cpuCopy.V);
    }

    template <typename SetupRegMem>
    void Test(Byte CPU::*Reg, CMPTestData t, SetupRegMem setupRegMem) {
        // given:
        cpu.*Reg = t.Register;
        cpu.C = !t.ExpectC;
        cpu.Z = !t.ExpectZ;
        cpu.N = !t.ExpectN;
        const s32 EXPECTED_CYCELS = setupRegMem(t.Operand);
        CPU cpuCopy = cpu;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCELS, mem);
        // then:
        EXPECT_EQ(actualCycles, EXPECTED_CYCELS);
        EXPECT_EQ(cpu.*Reg, t.Register);
        EXPECT_EQ(cpu.C, t.ExpectC);
        EXPECT_EQ(cpu.Z, t.ExpectZ);
        EXPECT_EQ(cpu.N, t.ExpectN);
        ExpectUnaffectedRegisters(cpuCopy);
    }

    void TestCMP_IM(CMPTestData t) {
        auto setImMemory = [this](Byte operand) {
            mem[0xFF00] = CPU::INS_CMP_IM;
            mem[0xFF01] = operand;
            return 2; // expected cycles;
        };
        Test(&CPU::A, t, setImMemory);
    }

    void TestCMP_ZP(CMPTestData t) {
        auto setImMemory = [this](Byte operand) {
            mem[0xFF00] = CPU::INS_CMP_ZP;
            mem[0xFF01] = 0x42;
            mem[0x0042] = operand;
            return 3; // expected cycles;
        };
        Test(&CPU::A, t, setImMemory);
    }

    void TestCMP_ZPX(CMPTestData t) {
        auto setImMemory = [this](Byte operand) {
            cpu.X = 0x01;
            mem[0xFF00] = CPU::INS_CMP_ZPX;
            mem[0xFF01] = 0x42;
            mem[0x0043] = operand;
            return 4; // expected cycles;
        };
        Test(&CPU::A, t, setImMemory);
    }

    void TestCMP_ABS(CMPTestData t) {
        auto setImMemory = [this](Byte operand) {
            mem[0xFF00] = CPU::INS_CMP_ABS;
            mem[0xFF01] = 0x00;
            mem[0xFF02] = 0x80;
            mem[0x8000] = operand;
            return 4; // expected cycles;
        };
        Test(&CPU::A, t, setImMemory);
    }

    void TestCMP_ABSX(CMPTestData t) {
        auto setImMemory = [this](Byte operand) {
            cpu.X = 0x01;
            mem[0xFF00] = CPU::INS_CMP_ABSX;
            mem[0xFF01] = 0x00;
            mem[0xFF02] = 0x80;
            mem[0x8001] = operand;
            return 4; // expected cycles;
        };
        Test(&CPU::A, t, setImMemory);
    }

    void TestCMP_ABSY(CMPTestData t) {
        auto setImMemory = [this](Byte operand) {
            cpu.Y = 0x01;
            mem[0xFF00] = CPU::INS_CMP_ABSY;
            mem[0xFF01] = 0x00;
            mem[0xFF02] = 0x80;
            mem[0x8001] = operand;
            return 4; // expected cycles;
        };
        Test(&CPU::A, t, setImMemory);
    }

    void TestCMP_INDX(CMPTestData t) {
        auto setImMemory = [this](Byte operand) {
            cpu.X = 0x04;
            mem[0xFF00] = CPU::INS_CMP_INDX;
            mem[0xFF01] = 0x02;
            mem[0x0006] = 0x00; // 0x2 + 0x4
            mem[0x0007] = 0x80; 
            mem[0x8000] = operand;
            return 6; // expected cycles;
        };
        Test(&CPU::A, t, setImMemory);
    }

    void TestCMP_INDY(CMPTestData t) {
        auto setImMemory = [this](Byte operand) {
            cpu.Y = 0x04;
            mem[0xFF00] = CPU::INS_CMP_INDY;
            mem[0xFF01] = 0x02;
            mem[0x0002] = 0x00;
            mem[0x0003] = 0x80; 
            mem[0x8004] = operand; // 0x8000 + 0x04(Y)
            return 5; // expected cycles;
        };
        Test(&CPU::A, t, setImMemory);
    }

    void TestCPX_IM(CMPTestData t) {
        auto setImMemory = [this](Byte operand) {
            mem[0xFF00] = CPU::INS_CPX_IM;
            mem[0xFF01] = operand;
            return 2; // expected cycles;
        };
        Test(&CPU::X, t, setImMemory);
    }

    void TestCPX_ZP(CMPTestData t) {
        auto setImMemory = [this](Byte operand) {
            mem[0xFF00] = CPU::INS_CPX_ZP;
            mem[0xFF01] = 0x42;
            mem[0x0042] = operand;
            return 3; // expected cycles;
        };
        Test(&CPU::X, t, setImMemory);
    }

    void TestCPX_ABS(CMPTestData t) {
        auto setImMemory = [this](Byte operand) {
            mem[0xFF00] = CPU::INS_CPX_ABS;
            mem[0xFF01] = 0x00;
            mem[0xFF02] = 0x80;
            mem[0x8000] = operand;
            return 4; // expected cycles;
        };
        Test(&CPU::X, t, setImMemory);
    }

    void TestCPY_IM(CMPTestData t) {
        auto setImMemory = [this](Byte operand) {
            mem[0xFF00] = CPU::INS_CPY_IM;
            mem[0xFF01] = operand;
            return 2; // expected cycles;
        };
        Test(&CPU::Y, t, setImMemory);
    }

    void TestCPY_ZP(CMPTestData t) {
        auto setImMemory = [this](Byte operand) {
            mem[0xFF00] = CPU::INS_CPY_ZP;
            mem[0xFF01] = 0x42;
            mem[0x0042] = operand;
            return 3; // expected cycles;
        };
        Test(&CPU::Y, t, setImMemory);
    }

    void TestCPY_ABS(CMPTestData t) {
        auto setImMemory = [this](Byte operand) {
            mem[0xFF00] = CPU::INS_CPY_ABS;
            mem[0xFF01] = 0x00;
            mem[0xFF02] = 0x80;
            mem[0x8000] = operand;
            return 4; // expected cycles;
        };
        Test(&CPU::Y, t, setImMemory);
    }
};

TEST_F(CompareTests, CMP_IM_26_26) {
    TestCMP_IM(data_cmp_26_26);
}
TEST_F(CompareTests, CMP_IM_48_26) {
    TestCMP_IM(data_cmp_48_26);
}
TEST_F(CompareTests, CMP_IM_130_26) {
    TestCMP_IM(data_cmp_130_26);
}
TEST_F(CompareTests, CMP_IM_8_26) {
    TestCMP_IM(data_cmp_8_26);
}

TEST_F(CompareTests, CMP_ZP_26_26) {
    TestCMP_ZP(data_cmp_26_26);
}
TEST_F(CompareTests, CMP_ZP_48_26) {
    TestCMP_ZP(data_cmp_48_26);
}
TEST_F(CompareTests, CMP_ZP_130_26) {
    TestCMP_ZP(data_cmp_130_26);
}
TEST_F(CompareTests, CMP_ZP_8_26) {
    TestCMP_ZP(data_cmp_8_26);
}

TEST_F(CompareTests, CMP_ZPX_26_26) {
    TestCMP_ZPX(data_cmp_26_26);
}
TEST_F(CompareTests, CMP_ZPX_48_26) {
    TestCMP_ZPX(data_cmp_48_26);
}
TEST_F(CompareTests, CMP_ZPX_130_26) {
    TestCMP_ZPX(data_cmp_130_26);
}
TEST_F(CompareTests, CMP_ZPX_8_26) {
    TestCMP_ZPX(data_cmp_8_26);
}

TEST_F(CompareTests, CMP_ABS_26_26) {
    TestCMP_ABS(data_cmp_26_26);
}
TEST_F(CompareTests, CMP_ABS_48_26) {
    TestCMP_ABS(data_cmp_48_26);
}
TEST_F(CompareTests, CMP_ABS_130_26) {
    TestCMP_ABS(data_cmp_130_26);
}
TEST_F(CompareTests, CMP_ABS_8_26) {
    TestCMP_ABS(data_cmp_8_26);
}

TEST_F(CompareTests, CMP_ABSX_130_26) {
    TestCMP_ABSX(data_cmp_130_26);
}
TEST_F(CompareTests, CMP_ABSX_8_26) {
    TestCMP_ABSX(data_cmp_8_26);
}

TEST_F(CompareTests, CMP_ABSY_130_26) {
    TestCMP_ABSY(data_cmp_130_26);
}
TEST_F(CompareTests, CMP_ABSY_8_26) {
    TestCMP_ABSY(data_cmp_8_26);
}

TEST_F(CompareTests, CMP_INDX_130_26) {
    TestCMP_INDX(data_cmp_130_26);
}
TEST_F(CompareTests, CMP_INDX_8_26) {
    TestCMP_INDX(data_cmp_8_26);
}

TEST_F(CompareTests, CMP_INDY_130_26) {
    TestCMP_INDY(data_cmp_130_26);
}
TEST_F(CompareTests, CMP_INDY_8_26) {
    TestCMP_INDY(data_cmp_8_26);
}

TEST_F(CompareTests, CPX_IM_130_26) {
    TestCPX_IM(data_cmp_130_26);
}
TEST_F(CompareTests, CPX_IM_8_26) {
    TestCPX_IM(data_cmp_8_26);
}
TEST_F(CompareTests, CPX_ZP_130_26) {
    TestCPX_ZP(data_cmp_130_26);
}
TEST_F(CompareTests, CPX_ZP_8_26) {
    TestCPX_ZP(data_cmp_8_26);
}
TEST_F(CompareTests, CPX_ABS_130_26) {
    TestCPX_ABS(data_cmp_130_26);
}
TEST_F(CompareTests, CPX_ABS_8_26) {
    TestCPX_ABS(data_cmp_8_26);
}

TEST_F(CompareTests, CPY_IM_130_26) {
    TestCPY_IM(data_cmp_130_26);
}
TEST_F(CompareTests, CPY_IM_8_26) {
    TestCPY_IM(data_cmp_8_26);
}
TEST_F(CompareTests, CPY_ZP_130_26) {
    TestCPY_ZP(data_cmp_130_26);
}
TEST_F(CompareTests, CPY_ZP_8_26) {
    TestCPY_ZP(data_cmp_8_26);
}
TEST_F(CompareTests, CPY_ABS_130_26) {
    TestCPY_ABS(data_cmp_130_26);
}
TEST_F(CompareTests, CPY_ABS_8_26) {
    TestCPY_ABS(data_cmp_8_26);
}
