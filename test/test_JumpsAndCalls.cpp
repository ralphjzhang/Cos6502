#include <gtest/gtest.h>

#include "../core/cos6502.hpp"

using namespace cos6502;

struct JumpsAndCallsTests : public testing::Test {
    Mem mem;
    CPU cpu;

    virtual void SetUp() {
        cpu.Reset(0xFF00, mem);
    }

    virtual void TearDown() {
    }
};

TEST_F(JumpsAndCallsTests, CanJumpToASubroutineAndJumpBack) {
    // given:
    mem[0xFF00] = CPU::INS_JSR;
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    mem[0x8000] = CPU::INS_RTS;
    mem[0xFF03] = CPU::INS_LDA_IM;
    mem[0xFF04] = 0x42;
    constexpr s32 EXPECTED_CYCLES = 6 + 6 + 2;
    CPU cpuCopy = cpu;
    // when:
    const s32 actualCycles = cpu.Execute(EXPECTED_CYCLES, mem);
    // then:
    EXPECT_EQ(actualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(cpu.A, 0x42);

}
