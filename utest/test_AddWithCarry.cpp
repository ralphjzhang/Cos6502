#include <gtest/gtest.h>

#include "../core/cp6502.hpp"

using namespace cp6502;

struct AddWithCarryTests : public testing::Test {
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

    struct ADCTestData {
        bool Carry;
        Byte A;
        Byte Operand;

        Byte Answer;
        bool ExpectC;
        bool ExpectZ;
        bool ExpectV;
        bool ExpectN;
    };

    void TestAbs(ADCTestData t) {
        // given:
        cpu.C = t.Carry;
        cpu.A = t.A;
        cpu.Z = !t.ExpectZ;
        cpu.V = !t.ExpectV;
        cpu.N = !t.ExpectN;
        mem[0xFF00] = CPU::INS_ADC_ABS;
        mem[0xFF01] = 0x00;
        mem[0xFF02] = 0x80;
        mem[0x8000] = t.Operand;
        constexpr s32 EXPECTED_CYCELS = 4;
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

};

TEST_F(AddWithCarryTests, ADCAdd_0_0) {
    ADCTestData test = {
        .Carry = false,
        .A = 0,
        .Operand = 0,

        .Answer = 0,
        .ExpectC = false,
        .ExpectZ = true,
        .ExpectV = false,
        .ExpectN = false,
    };
    TestAbs(test);
}

TEST_F(AddWithCarryTests, ADCAdd_0_0WithCarry_1) {
    ADCTestData test = {
        .Carry = true,
        .A = 0,
        .Operand = 0,

        .Answer = 1,
        .ExpectC = false,
        .ExpectZ = false,
        .ExpectV = false,
        .ExpectN = false,
    };
    TestAbs(test);
}

TEST_F(AddWithCarryTests, ADCAdd_1_FF_CausingCarry) {
    ADCTestData test = {
        .Carry = false,
        .A = 0xFF,
        .Operand = 1,

        .Answer = 0,
        .ExpectC = true,
        .ExpectZ = true,
        .ExpectV = false,
        .ExpectN = false,
    };
    TestAbs(test);
}

TEST_F(AddWithCarryTests, ADCSetNWhenAnswerNegative) {
    ADCTestData test = {
        .Carry = false,
        .A = 0,
        .Operand = Byte(-1),

        .Answer = Byte(-1),
        .ExpectC = false,
        .ExpectZ = false,
        .ExpectV = false,
        .ExpectN = true,
    };
    TestAbs(test);
}

TEST_F(AddWithCarryTests, ADCNegativeOverflow) {
    ADCTestData test = {
        .Carry = false,
        .A = Byte(-128),
        .Operand = Byte(-1),

        .Answer = 127,
        .ExpectC = true,
        .ExpectZ = false,
        .ExpectV = true,
        .ExpectN = false,
    };
    TestAbs(test);
}

TEST_F(AddWithCarryTests, ADCPositiveOverflow) {
    ADCTestData test = {
        .Carry = false,
        .A = Byte(127),
        .Operand = Byte(1),

        .Answer = Byte(-128),
        .ExpectC = false,
        .ExpectZ = false,
        .ExpectV = true,
        .ExpectN = true,
    };
    TestAbs(test);
}

TEST_F(AddWithCarryTests, ADCAdd_42_6_Carry_1) {
    ADCTestData test = {
        .Carry = true,
        .A = 42,
        .Operand = 6,

        .Answer = 49,
        .ExpectC = false,
        .ExpectZ = false,
        .ExpectV = false,
        .ExpectN = false,
    };
    TestAbs(test);
}

TEST_F(AddWithCarryTests, ADCAdd_20_n23_Carry_1) {
    ADCTestData test = {
        .Carry = 1,
        .A = 20,
        .Operand = Byte(-23),

        .Answer = Byte(-2),
        .ExpectC = false,
        .ExpectZ = false,
        .ExpectV = false,
        .ExpectN = true,
    };
    TestAbs(test);
}

TEST_F(AddWithCarryTests, ADCAdd_n20_23_Carry_1) {
    ADCTestData test = {
        .Carry = 1,
        .A = Byte(-20),
        .Operand = 23,

        .Answer = 4,
        .ExpectC = true,
        .ExpectZ = false,
        .ExpectV = false,
        .ExpectN = false,
    };
    TestAbs(test);
}

TEST_F(AddWithCarryTests, ADCCarryAndNegativeOverflow) {
    // A: 10000000 -128
    // O: 11111111 -1
    // C: 00000001 1
    ADCTestData test = {
        .Carry = 1,
        .A = Byte(-128),
        .Operand = Byte(-1),

        .Answer = Byte(-128),
        .ExpectC = true,
        .ExpectZ = false,
        .ExpectV = false,
        .ExpectN = true,
    };
    TestAbs(test);
}

TEST_F(AddWithCarryTests, ADCCarryAndPositiveOverflow) {
    // A: 01111111 127
    // O: 00000001 1
    // C: 00000001 1
    // R: 10000001 -127
    ADCTestData test = {
        .Carry = 1,
        .A = 127,
        .Operand = 1,

        .Answer = Byte(-127),
        .ExpectC = false,
        .ExpectZ = false,
        .ExpectV = true,
        .ExpectN = true,
    };
    TestAbs(test);
}
