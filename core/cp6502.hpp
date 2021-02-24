#pragma once
#include <stdio.h>
#include <stdlib.h>

namespace cp6502 
{
using SByte = signed char;
using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;
using s32 = signed int;

constexpr Byte CarryFlag        = 0b00000001;
constexpr Byte ZeroFlag         = 0b00000010;
constexpr Byte InterruptFlag    = 0b00000100;
constexpr Byte DecimalFlag      = 0b00001000;
constexpr Byte BreakFlag        = 0b00010000;
constexpr Byte UnusedFlag       = 0b00100000;
constexpr Byte OverflowFlag     = 0b00100000;
constexpr Byte NegativeFlag     = 0b10000000;

const static Word StackBase = 0x0100;

struct Mem;
struct CPU;
}

struct cp6502::Mem {
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];

    void Initialise() {
        for (u32 i = 0; i < MAX_MEM; ++i)
            Data[i] = 0;
    }

    Byte operator[](u32 address) const {
        return Data[address];
    }

    Byte& operator[](u32 address) {
        return Data[address];
    }

};

struct cp6502::CPU {
    Word PC;        // program counter
    Byte SP;        // stack pointer

    Byte A, X, Y;   // registers

    union {
        Byte PS;
        struct {
            Byte C : 1;         // 0: Carry
            Byte Z : 1;         // 1: Zero
            Byte I : 1;         // 2: Interrupe
            Byte D : 1;         // 3: Decimal
            Byte B : 1;         // 4: Break
            Byte Unused : 1;    // 5: Unused
            Byte V : 1;         // 6: Overflow
            Byte N : 1;         // 7: Negative
        };
    };

    void Reset(Word pc, Mem &memory) {
        PC = pc;
        SP = 0xFF;
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
        memory.Initialise();
    }

    Byte FetchByte(s32& cycles, Mem const& memory) {
        Byte data = memory[PC++];
        --cycles;
        return data;
    }

    Word FetchWord(s32& cycles, Mem const& memory) {
        // 6502 is little endian;
        Word data = memory[PC++];
        data |= (memory[PC++] << 8);
        cycles -= 2;
        return data;
    }

    Byte ReadByte(s32& cycles, Word addr, Mem const& memory) {
        Byte data = memory[addr];
        --cycles;
        return data;
    }

    Word ReadWord(s32& cycles, Word addr, Mem const& memory) {
        Byte loByte = ReadByte(cycles, addr, memory);
        Byte hiByte = ReadByte(cycles, addr + 1, memory);
        return loByte | (hiByte << 8);
    }

    void WriteByte(Byte value, s32& cycles, Word addr, Mem& memory) {
        memory[addr] = value;
        --cycles;
    }

    void WriteWord(Word value, s32& cycles, u32 address, Mem& memory) {
        memory.Data[address] = value & 0xFF;
        memory.Data[address + 1] = (value >> 8);
        cycles -= 2;
    }

    Word SPToAddress() const {
        return StackBase + SP;
    }

    void PushPCMinusOneToStack(s32& cycles, Mem& memory) {
        --SP;
        WriteWord(PC-1, cycles, SPToAddress(), memory);
        --SP;
    }

    void PushPCPlusOneToStack(s32& cycles, Mem& memory) {
        --SP;
        WriteWord(PC+1, cycles, SPToAddress(), memory);
        --SP;
    }

    void PushPCToStack(s32& cycles, Mem& memory) {
        --SP;
        WriteWord(PC, cycles, SPToAddress(), memory);
        --SP;
    }

    void PushByteOntoStack(s32& cycles, Byte value, Mem& memory) {
        memory[SPToAddress()] = value;
        --cycles;
        --SP;
        --cycles;
    }

    Byte PopByteFromStack(s32& cycles, Mem& memory) {
        ++SP;
        Byte value = ReadByte(cycles, SPToAddress(), memory);
        cycles--;
        return value;
    }

    Word PopWordFromStack(s32& cycles, Mem& memory) {
        ++SP;
        Word addr = ReadWord(cycles, SPToAddress(), memory);
        ++SP;
        --cycles;
        return addr;
    }

    void LoadRegisterSetStatus(Byte reg) {
        Z = (reg == 0);
        N = (reg & 0b10000000) > 0;
    }

    static constexpr Byte
        // LDA
        INS_LDA_IM = 0xA9,
        INS_LDA_ZP = 0xA5,
        INS_LDA_ZPX = 0xB5,
        INS_LDA_ABS = 0xAD,
        INS_LDA_ABSX = 0xBD,
        INS_LDA_ABSY = 0xB9,
        INS_LDA_INDX = 0xA1,
        INS_LDA_INDY = 0xB1,
        // LDX
        INS_LDX_IM = 0xA2,
        INS_LDX_ZP = 0xA6,
        INS_LDX_ZPY = 0xB6,
        INS_LDX_ABS = 0xAE,
        INS_LDX_ABSY = 0xBE,
        // LDY
        INS_LDY_IM = 0xA0,
        INS_LDY_ZP = 0xA4,
        INS_LDY_ZPX = 0xB4,
        INS_LDY_ABS = 0xAC,
        INS_LDY_ABSX = 0xBC,
        // STA
        INS_STA_ZP = 0x85,
        INS_STA_ZPX = 0x95,
        INS_STA_ABS = 0x8D,
        INS_STA_ABSX = 0x9D,
        INS_STA_ABSY = 0x99,
        INS_STA_INDX = 0x81,
        INS_STA_INDY = 0x91,
        // STX
        INS_STX_ZP = 0x86,
        INS_STX_ZPY = 0x96,
        INS_STX_ABS = 0x8E,
        // STY
        INS_STY_ZP = 0x84,
        INS_STY_ZPX = 0x94,
        INS_STY_ABS = 0x8C,
        // Stack
        INS_TSX = 0xBA,
        INS_TXS = 0x9A,
        INS_PHA = 0x48,
        INS_PHP = 0x08,
        INS_PLA = 0x68,
        INS_PLP = 0x28,
        // AND
        INS_AND_IM = 0x29,
        INS_AND_ZP = 0x25,
        INS_AND_ZPX = 0x35,
        INS_AND_ABS = 0x2D,
        INS_AND_ABSX = 0x3D,
        INS_AND_ABSY = 0x39,
        INS_AND_INDX = 0x21,
        INS_AND_INDY = 0x31,
        // EOR
        INS_EOR_IM = 0x49,
        INS_EOR_ZP = 0x45,
        INS_EOR_ZPX = 0x55,
        INS_EOR_ABS = 0x4D,
        INS_EOR_ABSX = 0x5D,
        INS_EOR_ABSY = 0x59,
        INS_EOR_INDX = 0x41,
        INS_EOR_INDY = 0x51,
        // ORA
        INS_ORA_IM = 0x09,
        INS_ORA_ZP = 0x05,
        INS_ORA_ZPX = 0x15,
        INS_ORA_ABS = 0x0D,
        INS_ORA_ABSX = 0x1D,
        INS_ORA_ABSY = 0x19,
        INS_ORA_INDX = 0x01,
        INS_ORA_INDY = 0x11,
        // BIT
        INS_BIT_ZP = 0x24,
        INS_BIT_ABS = 0x2C,
        // Register Transfer
        INS_TAX = 0xAA,
        INS_TAY = 0xA8,
        INS_TXA = 0x8A,
        INS_TYA = 0x98,
        // Increment Decrement
        INS_INX = 0xE8,
        INS_INY = 0xC8,
        INS_DEX = 0xCA,
        INS_DEY = 0x88,
        INS_INC_ZP = 0xE6,
        INS_INC_ZPX = 0xF6,
        INS_INC_ABS = 0xEE,
        INS_INC_ABSX = 0xFE,
        INS_DEC_ZP = 0xC6,
        INS_DEC_ZPX = 0xD6,
        INS_DEC_ABS = 0xCE,
        INS_DEC_ABSX = 0xDE,
        // Branches
        INS_BEQ = 0xF0,
        INS_BNE = 0xD0,
        INS_BCC = 0x90,
        INS_BCS = 0xB0,
        INS_BMI = 0x30,
        INS_BPL = 0x10,
        INS_BVS = 0x70,
        INS_BVC = 0x50,
        // Status Flags Changes
        INS_CLC = 0x18,
        INS_CLD = 0xD8,
        INS_CLI = 0x58,
        INS_CLV = 0xB8,
        INS_SEC = 0x38,
        INS_SED = 0xF8,
        INS_SEI = 0x78,
        // Arithmetic
        INS_ADC_IM = 0x69,
        INS_ADC_ZP = 0x65,
        INS_ADC_ZPX = 0x75,
        INS_ADC_ABS = 0x6D,
        INS_ADC_ABSX = 0x7D,
        INS_ADC_ABSY = 0x79,
        INS_ADC_INDX = 0x61,
        INS_ADC_INDY = 0x71,
        // CMP
        INS_CMP_IM = 0xC9,
        INS_CMP_ZP = 0xC5,
        INS_CMP_ZPX = 0xD5,
        INS_CMP_ABS = 0xCD,
        INS_CMP_ABSX = 0xDD,
        INS_CMP_ABSY = 0xD9,
        INS_CMP_INDX = 0xC1,
        INS_CMP_INDY = 0xD1,
        // CPX
        INS_CPX_IM = 0xE0,
        INS_CPX_ZP = 0xE4,
        INS_CPX_ABS = 0xEC,
        // CPY
        INS_CPY_IM = 0xC0,
        INS_CPY_ZP = 0xC4,
        INS_CPY_ABS = 0xCC,
        // SBC
        INS_SBC_IM = 0xE9,
        INS_SBC_ZP = 0xE5,
        INS_SBC_ZPX = 0xF5,
        INS_SBC_ABS = 0xED,
        INS_SBC_ABSX = 0xFD,
        INS_SBC_ABSY = 0xF9,
        INS_SBC_INDX = 0xE1,
        INS_SBC_INDY = 0xF1,
        // Shift
        INS_ASL_ACC = 0x0A,
        INS_ASL_ZP = 0x06,
        INS_ASL_ZPX = 0x16,
        INS_ASL_ABS = 0x0E,
        INS_ASL_ABSX = 0x1E,
        INS_LSR_ACC = 0x4A,
        INS_LSR_ZP = 0x46,
        INS_LSR_ZPX = 0x56,
        INS_LSR_ABS = 0x4E,
        INS_LSR_ABSX = 0x5E,
        INS_ROL_ACC = 0x2A,
        INS_ROL_ZP = 0x26,
        INS_ROL_ZPX = 0x36,
        INS_ROL_ABS = 0x2E,
        INS_ROL_ABSX = 0x3E,
        INS_ROR_ACC = 0x6A,
        INS_ROR_ZP = 0x66,
        INS_ROR_ZPX = 0x76,
        INS_ROR_ABS = 0x6E,
        INS_ROR_ABSX = 0x7E,
        // Jumps And Calls
        INS_JSR = 0x20,
        INS_RTS = 0x60,
        INS_JMP_ABS = 0x4C,
        INS_JMP_IND = 0x6C,
        // System functions
        INS_BRK = 0x00,
        INS_RTI = 0x40,
        INS_NOP = 0xEA
        ;

    Word LoadProg(Byte* prog, u32 numBytes, Mem& memory);
    s32 Execute(s32 cycles, Mem& memory);

    void PrintStatus() const {
        printf("A: %d X: %d Y: %d\n", A, X, Y);
        printf("PC: %d SP: %d\n", PC, SP);
    }

    Word AddrZeroPage(s32& cycles, Mem const& memory) {
        Byte zeroPageAddr = FetchByte(cycles, memory);
        return zeroPageAddr;
    }

    Word AddrZeroPageXY(s32& cycles, Byte regXY, Mem const& memory) {
        Byte addr = FetchByte(cycles, memory);
        addr += regXY;
        --cycles;
        return addr;
    }

    Word AddrAbsolute(s32& cycles, Mem const& memory) {
        Word addr = FetchWord(cycles, memory);
        return addr;
    }

    Word AddrAbsoluteXY(s32& cycles, Byte regXY, Mem const& memory) {
        Word absAddr = FetchWord(cycles, memory);
        Word addr = absAddr + regXY;
        bool pageCrossed = (absAddr & 0xFF00) != (addr & 0xFF00);
        if (pageCrossed)
            --cycles;
        return addr;
    }

    Word AddrAbsoluteXY_5(s32& cycles, Byte regXY, Mem const& memory) {
        Word absAddr = FetchWord(cycles, memory);
        Word addr = absAddr + regXY;
        --cycles;
        return addr;
    }

    Word AddrIndirectX(s32& cycles, Mem const& memory) {
        Byte zpAddr = FetchByte(cycles, memory);
        zpAddr += X;
        --cycles;
        Word effectiveAddr = ReadWord(cycles, zpAddr, memory);
        return effectiveAddr;
    }

    Word AddrIndirectY(s32& cycles, Mem const& memory) {
        Byte zpAddr = FetchByte(cycles, memory);
        Word effectiveAddr = ReadWord(cycles, zpAddr, memory);
        Word effectiveAddrY = effectiveAddr + Y;
        const bool pageCrossed = (effectiveAddr & 0xFF00) != (effectiveAddrY & 0xFF00);
        if (pageCrossed)
            --cycles;
        return effectiveAddrY;
    }

    Word AddrIndirectY_6(s32& cycles, Mem const& memory) {
        Byte zpAddr = FetchByte(cycles, memory);
        Word effectiveAddr = ReadWord(cycles, zpAddr, memory);
        Word effectiveAddrY = effectiveAddr + Y;
        --cycles;
        return effectiveAddrY;
    }

};

