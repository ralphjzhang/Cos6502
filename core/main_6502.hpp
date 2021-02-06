#include <stdio.h>
#include <stdlib.h>

using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;
using s32 = signed int;

struct Mem {
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

    void WriteWord(Word value, u32 address, u32& cycles) {
        Data[address] = value & 0xFF;
        Data[address + 1] = (value >> 8);
        cycles -= 2;
    }
};

struct CPU {
    Word PC;        // program counter
    Word SP;        // stack poiter

    Byte A, X, Y;   // registers

    Byte C : 1;     // status flags
    Byte Z : 1;
    Byte I : 1;
    Byte D : 1;
    Byte B : 1;
    Byte V : 1;
    Byte N : 1;

    void Reset(Mem& memory) {
        PC = 0xFFFC;
        SP = 0x0100;
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
        memory.Initialise();
    }

    Byte FetchByte(u32& cycles, Mem& memory) {
        Byte data = memory[PC++];
        --cycles;
        return data;
    }

    Word FetchWord(u32& cycles, Mem& memory) {
        // 6502 is little endian;
        Word data = memory[PC++];
        data |= (memory[PC++] << 8);
        cycles -= 2;
        return data;
    }

    Byte ReadByte(u32& cycles, Byte addr, Mem& memory) {
        Byte data = memory[addr];
        --cycles;
        return data;
    }

    void LDASetStatus() {
        Z = (A == 0);
        N = (A & 0b10000000) > 0;
    }

    static constexpr Byte
        INS_LDA_IM = 0xA9,
        INS_LDA_ZP = 0xA5,
        INS_LDA_ZPX = 0xB5,
        INS_JSR = 0x20
        ;

    s32 Execute(u32 cycles, Mem& memory) {
        const u32 cyclesRequested = cycles;
        while (cycles > 0) {
            Byte ins = FetchByte(cycles, memory);
            switch (ins) {
                case INS_LDA_IM: {
                    Byte value = FetchByte(cycles, memory);
                    A = value;
                    LDASetStatus();
                } break;
                case INS_LDA_ZP: {
                    Byte zeroPageAddr = FetchByte(cycles, memory);
                    A = ReadByte(cycles, zeroPageAddr, memory);
                    LDASetStatus();
                } break;
                case INS_LDA_ZPX: {
                    Byte zeroPageAddr = FetchByte(cycles, memory);
                    zeroPageAddr += X;
                    --cycles;
                    A = ReadByte(cycles, zeroPageAddr, memory);
                    LDASetStatus();
                } break;
                case INS_JSR: {
                    Word subAddr = FetchWord(cycles, memory);
                    memory.WriteWord(PC - 1, SP, cycles);
                    PC = subAddr;
                    --cycles;
                } break;
                default: {
                    printf("Instruction not implemented: %d \n", ins);
                } break;
            }
        }
        return cyclesRequested - cycles;
    }
};

