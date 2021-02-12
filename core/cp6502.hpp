#pragma once
#include <stdio.h>
#include <stdlib.h>

namespace cp6502 
{
using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;
using s32 = signed int;

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
            Byte C : 1; // status flags
            Byte Z : 1;
            Byte I : 1;
            Byte D : 1;
            Byte B : 1;
            Byte Unused : 1;
            Byte V : 1;
            Byte N : 1;
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
        return 0x0100 + SP;
    }

    void PushPCToStack(s32& cycles, Mem& memory) {
        WriteWord(PC, cycles, SPToAddress(), memory);
        SP -= 2;
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
        cycles -= 2;
        return value;
    }

    Word PopWordFromStack(s32& cycles, Mem& memory) {
        SP += 2;
        Word addr = ReadWord(cycles, SPToAddress(), memory);
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
        // Jumps And Calls
        INS_JSR = 0x20,
        INS_RTS = 0x60,
        INS_JMP_ABS = 0x4C,
        INS_JMP_IND = 0x6C
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
        if (addr - absAddr >= 0xFF) // crosses page boundary
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
        if (effectiveAddrY - effectiveAddr >= 0xFF) // crosses page boundary
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

