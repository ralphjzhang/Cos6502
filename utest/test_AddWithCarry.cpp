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

    ADCTestData data_add_n20_23_carry_1 = {
        .Carry = 1,
        .A = Byte(-20),
        .Operand = 23,

        .Answer = 4,
        .ExpectC = true,
        .ExpectZ = false,
        .ExpectV = false,
        .ExpectN = false,
    };

    ADCTestData data_add_20_n23_carry_1 = {
        .Carry = 1,
        .A = 20,
        .Operand = Byte(-23),

        .Answer = Byte(-2),
        .ExpectC = false,
        .ExpectZ = false,
        .ExpectV = false,
        .ExpectN = true,
    };

    void TestIm(ADCTestData t) {
        // given:
        cpu.C = t.Carry;
        cpu.A = t.A;
        cpu.Z = !t.ExpectZ;
        cpu.V = !t.ExpectV;
        cpu.N = !t.ExpectN;
        mem[0xFF00] = CPU::INS_ADC_IM;
        mem[0xFF01] = t.Operand;
        constexpr s32 EXPECTED_CYCELS = 2;
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

    void TestZP(ADCTestData t) {
        // given:
        cpu.C = t.Carry;
        cpu.A = t.A;
        cpu.Z = !t.ExpectZ;
        cpu.V = !t.ExpectV;
        cpu.N = !t.ExpectN;
        mem[0xFF00] = CPU::INS_ADC_ZP;
        mem[0xFF01] = 0x42;
        mem[0x0042] = t.Operand;
        constexpr s32 EXPECTED_CYCELS = 3;
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

    void TestZPX(ADCTestData t) {
        // given:
        cpu.X = 0x01;
        cpu.C = t.Carry;
        cpu.A = t.A;
        cpu.Z = !t.ExpectZ;
        cpu.V = !t.ExpectV;
        cpu.N = !t.ExpectN;
        mem[0xFF00] = CPU::INS_ADC_ZPX;
        mem[0xFF01] = 0x42;
        mem[0x0043] = t.Operand;
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

    void TestAbsX(ADCTestData t) {
        // given:
        cpu.X = 0x01;
        cpu.C = t.Carry;
        cpu.A = t.A;
        cpu.Z = !t.ExpectZ;
        cpu.V = !t.ExpectV;
        cpu.N = !t.ExpectN;
        mem[0xFF00] = CPU::INS_ADC_ABSX;
        mem[0xFF01] = 0x00;
        mem[0xFF02] = 0x80;
        mem[0x8001] = t.Operand;
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

    void TestAbsY(ADCTestData t) {
        // given:
        cpu.Y = 0x01;
        cpu.C = t.Carry;
        cpu.A = t.A;
        cpu.Z = !t.ExpectZ;
        cpu.V = !t.ExpectV;
        cpu.N = !t.ExpectN;
        mem[0xFF00] = CPU::INS_ADC_ABSY;
        mem[0xFF01] = 0x00;
        mem[0xFF02] = 0x80;
        mem[0x8001] = t.Operand;
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

    void TestIndX(ADCTestData t) {
        // given:
        cpu.X = 0x04;
        cpu.C = t.Carry;
        cpu.A = t.A;
        cpu.Z = !t.ExpectZ;
        cpu.V = !t.ExpectV;
        cpu.N = !t.ExpectN;
        mem[0xFF00] = CPU::INS_ADC_INDX;
        mem[0xFF01] = 0x02;
        mem[0x0006] = 0x00; // 0x2 + 0x4
        mem[0x0007] = 0x80; 
        mem[0x8000] = t.Operand;
        constexpr s32 EXPECTED_CYCELS = 6;
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

    void TestIndY(ADCTestData t) {
        // given:
        cpu.Y = 0x04;
        cpu.C = t.Carry;
        cpu.A = t.A;
        cpu.Z = !t.ExpectZ;
        cpu.V = !t.ExpectV;
        cpu.N = !t.ExpectN;
        mem[0xFF00] = CPU::INS_ADC_INDY;
        mem[0xFF01] = 0x02;
        mem[0x0002] = 0x00;
        mem[0x0003] = 0x80; 
        mem[0x8004] = t.Operand; // 0x8000 + 0x04(Y)
        constexpr s32 EXPECTED_CYCELS = 5;
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

TEST_F(AddWithCarryTests, ADCAbsAdd_0_0) {
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

TEST_F(AddWithCarryTests, ADCAbsAdd_0_0WithCarry_1) {
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

TEST_F(AddWithCarryTests, ADCAbsAdd_1_FF_CausingCarry) {
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

TEST_F(AddWithCarryTests, ADCAbsSetNWhenAnswerNegative) {
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

TEST_F(AddWithCarryTests, ADCAbsNegativeOverflow) {
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

TEST_F(AddWithCarryTests, ADCAbsPositiveOverflow) {
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

TEST_F(AddWithCarryTests, ADCAbsAdd_42_6_Carry_1) {
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

TEST_F(AddWithCarryTests, ADCAbsAdd_20_n23_Carry_1) {
    TestAbs(data_add_20_n23_carry_1);
}

TEST_F(AddWithCarryTests, ADCAbsAdd_n20_23_Carry_1) {
    TestAbs(data_add_n20_23_carry_1);
}

TEST_F(AddWithCarryTests, ADCAbsCarryAndNegativeOverflow) {
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

TEST_F(AddWithCarryTests, ADCAbsCarryAndPositiveOverflow) {
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

TEST_F(AddWithCarryTests, ADCImAdd_20_n23_Carry_1) {
    TestIm(data_add_20_n23_carry_1);
}
TEST_F(AddWithCarryTests, ADCImAdd_n20_23_Carry_1) {
    TestIm(data_add_n20_23_carry_1);
}

TEST_F(AddWithCarryTests, ADCZPAdd_20_n23_Carry_1) {
    TestZP(data_add_20_n23_carry_1);
}
TEST_F(AddWithCarryTests, ADCZPAdd_n20_23_Carry_1) {
    TestZP(data_add_n20_23_carry_1);
}

TEST_F(AddWithCarryTests, ADCZPXAdd_20_n23_Carry_1) {
    TestZPX(data_add_20_n23_carry_1);
}
TEST_F(AddWithCarryTests, ADCZPXAdd_n20_23_Carry_1) {
    TestZPX(data_add_n20_23_carry_1);
}

TEST_F(AddWithCarryTests, ADCAbsXAdd_20_n23_Carry_1) {
    TestAbsX(data_add_20_n23_carry_1);
}
TEST_F(AddWithCarryTests, ADCAbsXAdd_n20_23_Carry_1) {
    TestAbsX(data_add_n20_23_carry_1);
}

TEST_F(AddWithCarryTests, ADCAbsYAdd_20_n23_Carry_1) {
    TestAbsY(data_add_20_n23_carry_1);
}
TEST_F(AddWithCarryTests, ADCAbsYAdd_n20_23_Carry_1) {
    TestAbsY(data_add_n20_23_carry_1);
}

TEST_F(AddWithCarryTests, ADCIndXAdd_20_n23_Carry_1) {
    TestIndX(data_add_20_n23_carry_1);
}
TEST_F(AddWithCarryTests, ADCIndXAdd_n20_23_Carry_1) {
    TestIndX(data_add_n20_23_carry_1);
}

TEST_F(AddWithCarryTests, ADCIndYAdd_20_n23_Carry_1) {
    TestIndY(data_add_20_n23_carry_1);
}
TEST_F(AddWithCarryTests, ADCIndYAdd_n20_23_Carry_1) {
    TestIndY(data_add_n20_23_carry_1);
}