#include <gtest/gtest.h>

#include "../core/cp6502.hpp"

using namespace cp6502;

struct StatusFlagChangesTests : public testing::Test {
    Mem mem;
    CPU cpu;

    virtual void SetUp() {
        cpu.Reset(0xFF00, mem);
    }

    virtual void TearDown() {
    }

    template <typename GetAndSetFlag>
    void FlagCleared(Byte opcode, GetAndSetFlag getAndSetFlag) {
        // given:
        cpu.PS = 0xFF;
        mem[0xFF00] = opcode;
        constexpr s32 EXPECTED_CYCLES = 2;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
        EXPECT_EQ(getAndSetFlag(), 0);
        EXPECT_EQ(cpu.PS, 0xFF);    // if all other flags are not affected by the instruction, PS should be back to 0xFF
    }

    template <typename GetAndClearFlag>
    void FlagSet(Byte opcode, GetAndClearFlag getAndClearFlag) {
        // given:
        cpu.PS = 0x00;
        mem[0xFF00] = opcode;
        constexpr s32 EXPECTED_CYCLES = 2;
        // when:
        const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
        // then:
        EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
        EXPECT_EQ(getAndClearFlag(), 1);
        EXPECT_EQ(cpu.PS, 0x00);    // if all other flags are not affected by the instruction, PS should be back to 0x00
    }

};

TEST_F(StatusFlagChangesTests, CLCCanClearCRegister) {
    FlagCleared(CPU::INS_CLC, [this]() {
        auto res = cpu.C;
        cpu.C = 1;
        return res;
    });
}

TEST_F(StatusFlagChangesTests, CLDCanClearDRegister) {
    FlagCleared(CPU::INS_CLD, [this]() {
        auto res = cpu.D;
        cpu.D = 1;
        return res;
    });
}

TEST_F(StatusFlagChangesTests, CLICanClearIRegister) {
    FlagCleared(CPU::INS_CLI, [this]() {
        auto res = cpu.I;
        cpu.I = 1;
        return res;
    });
}

TEST_F(StatusFlagChangesTests, CLVCanClearVRegister) {
    FlagCleared(CPU::INS_CLV, [this]() {
        auto res = cpu.V;
        cpu.V = 1;
        return res;
    });
}

TEST_F(StatusFlagChangesTests, SECCanSetCRegister) {
    FlagSet(CPU::INS_SEC, [this]() {
        auto res = cpu.C;
        cpu.C = 0;
        return res;
    });
}

TEST_F(StatusFlagChangesTests, SEDCanSetDRegister) {
    FlagSet(CPU::INS_SED, [this]() {
        auto res = cpu.D;
        cpu.D = 0;
        return res;
    });
}

TEST_F(StatusFlagChangesTests, SEICanSetIRegister) {
    FlagSet(CPU::INS_SEI, [this]() {
        auto res = cpu.I;
        cpu.I = 0;
        return res;
    });
}

TEST_F(StatusFlagChangesTests, NOP) {
    // given:
    mem[0xFF00] = CPU::INS_NOP;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
    EXPECT_EQ(cpu.PC, cpuCopy.PC + 1);
    EXPECT_EQ(cpu.A, cpuCopy.A);
    EXPECT_EQ(cpu.X, cpuCopy.X);
    EXPECT_EQ(cpu.Y, cpuCopy.Y);
}