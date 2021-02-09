#include "cp6502.hpp"

namespace cp6502 {


s32 CPU::Execute(s32 cycles, Mem& memory) {
    auto LoadRegister = [&cycles, &memory, this](Word addr, Byte& reg) {
        reg = ReadByte(cycles, addr, memory);
        LoadRegisterSetStatus(reg);
    };

    const s32 cyclesRequested = cycles;
    while (cycles > 0) {
        Byte ins = FetchByte(cycles, memory);
        switch (ins) {
            case INS_LDA_IM: {
                A = FetchByte(cycles, memory);
                LoadRegisterSetStatus(A);
            } break;
            case INS_LDX_IM: {
                X = FetchByte(cycles, memory);
                LoadRegisterSetStatus(X);
            } break;
            case INS_LDY_IM: {
                Y = FetchByte(cycles, memory);
                LoadRegisterSetStatus(Y);
            } break;
            case INS_LDA_ZP: {
                Word addr = AddrZeroPage(cycles, memory);
                LoadRegister(addr, A);
            } break;
            case INS_LDX_ZP: {
                Word addr = AddrZeroPage(cycles, memory);
                LoadRegister(addr, X);
            } break;
            case INS_LDY_ZP: {
                Word addr = AddrZeroPage(cycles, memory);
                LoadRegister(addr, Y);
            } break;
            case INS_LDA_ZPX: {
                Word addr = AddrZeroPageXY(cycles, X, memory);
                LoadRegister(addr, A);
            } break;
            case INS_LDX_ZPY: {
                Word addr = AddrZeroPageXY(cycles, Y, memory);
                LoadRegister(addr, X);
            } break;
            case INS_LDY_ZPX: {
                Word addr = AddrZeroPageXY(cycles, X, memory);
                LoadRegister(addr, Y);
            } break;
            case INS_LDA_ABS: {
                Word addr = AddrAbsolute(cycles, memory);
                LoadRegister(addr, A);
            } break;
            case INS_LDX_ABS: {
                Word addr = AddrAbsolute(cycles, memory);
                LoadRegister(addr, X);
            } break;
            case INS_LDY_ABS: {
                Word addr = AddrAbsolute(cycles, memory);
                LoadRegister(addr, Y);
            } break;
            case INS_LDA_ABSX: {
                Word addr = AddrAbsoluteXY(cycles, X, memory);
                LoadRegister(addr, A);
            } break;
            case INS_LDA_ABSY: {
                Word addr = AddrAbsoluteXY(cycles, Y, memory);
                LoadRegister(addr, A);
            } break;
            case INS_LDX_ABSY: {
                Word addr = AddrAbsoluteXY(cycles, Y, memory);
                LoadRegister(addr, X);
            } break;
            case INS_LDY_ABSX: {
                Word addr = AddrAbsoluteXY(cycles, X, memory);
                LoadRegister(addr, Y);
            } break;
            case INS_LDA_INDX: {
                Word addr = AddrIndirectX(cycles, memory);
                A = ReadByte(cycles, addr, memory);
                LoadRegisterSetStatus(A);
            } break;
            case INS_LDA_INDY: {
                Word addr = AddrIndirectY(cycles, memory);
                A = ReadByte(cycles, addr, memory);
                LoadRegisterSetStatus(A);
            } break;
            case INS_STA_ZP: {
                Word addr = AddrZeroPage(cycles, memory);
                WriteByte(A, cycles, addr, memory);
            } break;
            case INS_STX_ZP: {
                Word addr = AddrZeroPage(cycles, memory);
                WriteByte(X, cycles, addr, memory);
            } break;
            case INS_STY_ZP: {
                Word addr = AddrZeroPage(cycles, memory);
                WriteByte(Y, cycles, addr, memory);
            } break;
            case INS_STA_ABS: {
                Word addr = AddrAbsolute(cycles, memory);
                WriteByte(A, cycles, addr, memory);
            } break;
            case INS_STX_ABS: {
                Word addr = AddrAbsolute(cycles, memory);
                WriteByte(X, cycles, addr, memory);
            } break;
            case INS_STY_ABS: {
                Word addr = AddrAbsolute(cycles, memory);
                WriteByte(Y, cycles, addr, memory);
            } break;
            case INS_STA_ZPX: {
                Word addr = AddrZeroPageXY(cycles, X, memory);
                WriteByte(A, cycles, addr, memory);
            } break;
            case INS_STX_ZPY: {
                Word addr = AddrZeroPageXY(cycles, Y, memory);
                WriteByte(X, cycles, addr, memory);
            } break;
            case INS_STY_ZPX: {
                Word addr = AddrZeroPageXY(cycles, X, memory);
                WriteByte(Y, cycles, addr, memory);
            } break;
            case INS_STA_ABSX: {
                Word addr = AddrAbsoluteXY_5(cycles, X, memory);
                WriteByte(A, cycles, addr, memory);
            } break;
            case INS_STA_ABSY: {
                Word addr = AddrAbsoluteXY_5(cycles, Y, memory);
                WriteByte(A, cycles, addr, memory);
            } break;
            case INS_STA_INDX: {
                Word addr = AddrIndirectX(cycles, memory);
                WriteByte(A, cycles, addr, memory);
            } break;
            case INS_STA_INDY: {
                Word addr = AddrIndirectY_6(cycles, memory);
                WriteByte(A, cycles, addr, memory);
            } break;
            case INS_JSR: {
                Word subAddr = FetchWord(cycles, memory);
                PushPCToStack(cycles, memory);
                PC = subAddr;
                --cycles;
            } break;
            case INS_RTS: {
                Word retAddr = PopWordFromStack(cycles, memory);
                PC = retAddr;
                cycles -= 2;
            } break;
            default: {
                throw "Instruction not implemented";
            } break;
        }
    }
    return cyclesRequested - cycles;
}

} // namespace cp6502
