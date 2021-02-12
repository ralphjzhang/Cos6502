#include "cp6502.hpp"

namespace cp6502 {


s32 CPU::Execute(s32 cycles, Mem& memory) {
    auto LoadRegister = [&cycles, &memory, this](Word addr, Byte& reg) {
        reg = ReadByte(cycles, addr, memory);
        LoadRegisterSetStatus(reg);
    };

    auto And = [&cycles, &memory, this](Word addr) {
        A &= ReadByte(cycles, addr, memory);
        LoadRegisterSetStatus(A);
    };
    auto Eor = [&cycles, &memory, this](Word addr) {
        A ^= ReadByte(cycles, addr, memory);
        LoadRegisterSetStatus(A);
    };
    auto Ora = [&cycles, &memory, this](Word addr) {
        A |= ReadByte(cycles, addr, memory);
        LoadRegisterSetStatus(A);
    };
    auto Bit = [&cycles, &memory, this](Word addr) {
        Byte value = ReadByte(cycles, addr, memory);
        Z = (A & value) == 0;
        N = (value >> 7) & 1;
        V = (value >> 6) & 1;
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
            case INS_JMP_ABS: {
                Word addr = AddrAbsolute(cycles, memory);
                PC = addr;
            } break;
            case INS_JMP_IND: {
                Word addr = AddrAbsolute(cycles, memory);
                addr = ReadWord(cycles, addr, memory);
                PC = addr;
            } break;
            // Stacks
            case INS_TSX: {
                X = SP;
                --cycles;
                LoadRegisterSetStatus(X);
            } break;
            case INS_TXS: {
                SP = X;
                --cycles;
            } break;
            case INS_PHA: {
                PushByteOntoStack(cycles, A, memory);
            } break;
            case INS_PLA: {
                A = PopByteFromStack(cycles, memory);
                LoadRegisterSetStatus(A);
            } break;
            case INS_PHP: {
                PushByteOntoStack(cycles, PS, memory);
            } break;
            case INS_PLP: {
                PS = PopByteFromStack(cycles, memory);
            } break;
            // Logicals
            case INS_AND_IM: {
                A = A & FetchByte(cycles, memory);
                LoadRegisterSetStatus(A);
            } break;
            case INS_EOR_IM: {
                A = A ^ FetchByte(cycles, memory);
                LoadRegisterSetStatus(A);
            } break;
            case INS_ORA_IM: {
                A = A | FetchByte(cycles, memory);
                LoadRegisterSetStatus(A);
            } break;
            case INS_AND_ZP: {
                Word addr = AddrZeroPage(cycles, memory);
                And(addr);
            } break;
            case INS_EOR_ZP: {
                Word addr = AddrZeroPage(cycles, memory);
                Eor(addr);
            } break;
            case INS_ORA_ZP: {
                Word addr = AddrZeroPage(cycles, memory);
                Ora(addr);
            } break;
            case INS_AND_ZPX: {
                Word addr = AddrZeroPageXY(cycles, X, memory);
                And(addr);
            } break;
            case INS_EOR_ZPX: {
                Word addr = AddrZeroPageXY(cycles, X, memory);
                Eor(addr);
            } break;
            case INS_ORA_ZPX: {
                Word addr = AddrZeroPageXY(cycles, X, memory);
                Ora(addr);
            } break;
            case INS_AND_ABS: {
                Word addr = AddrAbsolute(cycles, memory);
                And(addr);
            } break;
            case INS_EOR_ABS: {
                Word addr = AddrAbsolute(cycles, memory);
                Eor(addr);
            } break;
            case INS_ORA_ABS: {
                Word addr = AddrAbsolute(cycles, memory);
                Ora(addr);
            } break;
            case INS_AND_ABSX: {
                Word addr = AddrAbsoluteXY(cycles, X, memory);
                And(addr);
            } break;
            case INS_EOR_ABSX: {
                Word addr = AddrAbsoluteXY(cycles, X, memory);
                Eor(addr);
            } break;
            case INS_ORA_ABSX: {
                Word addr = AddrAbsoluteXY(cycles, X, memory);
                Ora(addr);
            } break;
            case INS_AND_ABSY: {
                Word addr = AddrAbsoluteXY(cycles, Y, memory);
                And(addr);
            } break;
            case INS_EOR_ABSY: {
                Word addr = AddrAbsoluteXY(cycles, Y, memory);
                Eor(addr);
            } break;
            case INS_ORA_ABSY: {
                Word addr = AddrAbsoluteXY(cycles, Y, memory);
                Ora(addr);
            } break;
            case INS_AND_INDX: {
                Word addr = AddrIndirectX(cycles, memory);
                And(addr);
            } break;
            case INS_EOR_INDX: {
                Word addr = AddrIndirectX(cycles, memory);
                Eor(addr);
            } break;
            case INS_ORA_INDX: {
                Word addr = AddrIndirectX(cycles, memory);
                Ora(addr);
            } break;
            case INS_AND_INDY: {
                Word addr = AddrIndirectY(cycles, memory);
                And(addr);
            } break;
            case INS_EOR_INDY: {
                Word addr = AddrIndirectY(cycles, memory);
                Eor(addr);
            } break;
            case INS_ORA_INDY: {
                Word addr = AddrIndirectY(cycles, memory);
                Ora(addr);
            } break;
            case INS_BIT_ZP: {
                Word addr = AddrZeroPage(cycles, memory);
                Bit(addr);
            } break;
            case INS_BIT_ABS: {
                Word addr = AddrAbsolute(cycles, memory);
                Bit(addr);
            } break;

            default: {
                throw "Instruction not implemented";
            } break;
        }
    }
    return cyclesRequested - cycles;
}

} // namespace cp6502
