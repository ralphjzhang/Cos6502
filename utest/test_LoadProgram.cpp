#include <gtest/gtest.h>

#include "../core/cp6502.hpp"

using namespace cp6502;

struct LoadProgramTests : public testing::Test {
    Mem mem;
    CPU cpu;
    constexpr static s32 NumByteTestProg = 14;
    Byte TestProg[NumByteTestProg] = { 0x00,0x10,0xA9,0xFF,0x85,0x90,0x8D,0x00,0x80,0x49,0xCC,0x4C,0x02,0x10 };

    virtual void SetUp() {
        cpu.Reset(0xFFFC, mem);
    }

    virtual void TearDown() {
    }
};

 
TEST_F(LoadProgramTests, TestLoadProgramIntoMemory) {
    // given:
    // when:
    cpu.LoadProg(TestProg, NumByteTestProg, mem);
    // then:
    EXPECT_EQ(mem[0x0FFF],0x0);
    EXPECT_EQ(mem[0x1000],0xA9);
    EXPECT_EQ(mem[0x1001],0xFF);
    EXPECT_EQ(mem[0x1002],0x85);
    // ...
    EXPECT_EQ(mem[0x1009], 0x4C);
    EXPECT_EQ(mem[0x100A], 0x02);
    EXPECT_EQ(mem[0x100B], 0x10);
    EXPECT_EQ(mem[0x100C], 0x0);
    // then:
}

TEST_F(LoadProgramTests, TestLoadProgramAndExecute) {
    // given:
    // when:
    cpu.PC = cpu.LoadProg(TestProg, NumByteTestProg, mem);
    for (s32 clock = 1; clock > 0; --clock)
        clock -= cpu.Execute(1, mem);
    // then;
    EXPECT_EQ(cpu.A, 0xFF);
}
