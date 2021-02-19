#include "cp6502.hpp"

namespace cp6502 {

Word CPU::LoadProg(Byte* prog, u32 numBytes, Mem& memory) {
    if (prog) {
        u32 at = 0;
        Word loadAddr = prog[at++] | (prog[at++] << 8);
        for (Word i = loadAddr; i < loadAddr + numBytes - 2; ++i)
            memory[i] = prog[at++];
        return loadAddr;
    }
    return 0;
}

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

    auto Inc = [&cycles, &memory, this](Word addr) {
        Byte value = ReadByte(cycles, addr, memory);
        WriteByte(++value, cycles, addr, memory);
        LoadRegisterSetStatus(value);
    };
    auto Dec = [&cycles, &memory, this](Word addr) {
        Byte value = ReadByte(cycles, addr, memory);
        WriteByte(--value, cycles, addr, memory);
        LoadRegisterSetStatus(value);
    };

    auto BranchIf = [&cycles, &memory, this](auto predicate) {
        Byte offset = FetchByte(cycles, memory);
        if (predicate())
        {
            const Word oldPC = PC;
            PC += static_cast<SByte>(offset);
            cycles--;
            const bool pageChanged = (PC >> 8) != (oldPC >> 8);
            if (pageChanged)
                cycles -= 2;
        }
    };

    auto ADC = [this](Byte operand) {
        if (D) throw "Decimal not implemented";
        Byte ASign = (A & NFlagBit);
        Byte operandSign = (operand & NFlagBit);
        Word sum = A;
        sum += C;
        sum += operand;
        A = (sum & 0xFF);
        C = sum > 0xFF;
        Z = (A == 0);
        // overflow:
        // two operand have same sign, but the result has different sign
        V = (ASign == operandSign) && ((A & NFlagBit) != ASign);
        N = (A & NFlagBit) > 0;
    };

    auto CMP = [this](Byte operand) {
        Byte diff = A - operand;
        C = A >= operand;
        Z = A == operand;
        N = (diff & NFlagBit) > 0;
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
            case INS_TAX: {
                X = A;
                LoadRegisterSetStatus(X);
                cycles -= 2;
            } break;
            case INS_TAY: {
                Y = A;
                LoadRegisterSetStatus(Y);
                cycles -= 2;
            } break;
            case INS_TXA: {
                A = X;
                LoadRegisterSetStatus(A);
                cycles -= 2;
            } break;
            case INS_TYA: {
                A = Y;
                LoadRegisterSetStatus(A);
                cycles -= 2;
            } break;
            case INS_INX: {
                ++X;
                LoadRegisterSetStatus(X);
                cycles -= 2;
            } break;
            case INS_INY: {
                ++Y;
                LoadRegisterSetStatus(Y);
                cycles -= 2;
            } break;
            case INS_DEX: {
                --X;
                LoadRegisterSetStatus(X);
                cycles -= 2;
            } break;
            case INS_DEY: {
                --Y;
                LoadRegisterSetStatus(Y);
                cycles -= 2;
            } break;
            case INS_INC_ZP: {
                Word addr = AddrZeroPage(cycles, memory);
                Inc(addr);
                cycles -= 2;
            } break;
            case INS_INC_ZPX: {
                Word addr = AddrZeroPageXY(cycles, X, memory);
                Inc(addr);
                cycles -= 2;
            } break;
            case INS_INC_ABS: {
                Word addr = AddrAbsolute(cycles, memory);
                Inc(addr);
                cycles -= 2;
            } break;
            case INS_INC_ABSX: {
                Word addr = AddrAbsoluteXY(cycles, X, memory);
                Inc(addr);
                cycles -= 2;
            } break;
            case INS_DEC_ZP: {
                Word addr = AddrZeroPage(cycles, memory);
                Dec(addr);
                cycles -= 2;
            } break;
            case INS_DEC_ZPX: {
                Word addr = AddrZeroPageXY(cycles, X, memory);
                Dec(addr);
                cycles -= 2;
            } break;
            case INS_DEC_ABS: {
                Word addr = AddrAbsolute(cycles, memory);
                Dec(addr);
                cycles -= 2;
            } break;
            case INS_DEC_ABSX: {
                Word addr = AddrAbsoluteXY(cycles, X, memory);
                Dec(addr);
                cycles -= 2;
            } break;
            case INS_BEQ: {
                BranchIf([this]() -> bool { return Z; });
            } break;
            case INS_BNE: {
                BranchIf([this]() -> bool { return !Z; });
            } break;
            case INS_BCC: {
                BranchIf([this]() -> bool { return !C; });
            } break;
            case INS_BCS: {
                BranchIf([this]() -> bool { return C; });
            } break;
            case INS_BMI: {
                BranchIf([this]() -> bool { return N; });
            } break;
            case INS_BPL: {
                BranchIf([this]() -> bool { return !N; });
            } break;
            case INS_BVS: {
                BranchIf([this]() -> bool { return V; });
            } break;
            case INS_BVC: {
                BranchIf([this]() -> bool { return !V; });
            } break;
            case INS_CLC: {
                C = 0;
                --cycles;
            } break;
            case INS_CLD: {
                D = 0;
                --cycles;
            } break;
            case INS_CLI: {
                I = 0;
                --cycles;
            } break;
            case INS_CLV: {
                V = 0;
                --cycles;
            } break;
            case INS_SEC: {
                C = 1;
                --cycles;
            } break;
            case INS_SED: {
                D = 1;
                --cycles;
            } break;
            case INS_SEI: {
                I = 1;
                --cycles;
            } break;
            case INS_NOP: {
                --cycles;
            } break;
            case INS_ADC_IM: {
                Byte operand = FetchByte(cycles, memory);
                ADC(operand);
            } break;
            case INS_ADC_ZP: {
                Word addr = AddrZeroPage(cycles, memory);
                Byte operand = ReadByte(cycles, addr, memory);
                ADC(operand);
            } break;
            case INS_ADC_ZPX: {
                Word addr = AddrZeroPageXY(cycles, X, memory);
                Byte operand = ReadByte(cycles, addr, memory);
                ADC(operand);
            } break;
            case INS_ADC_ABS: {
                Word addr = AddrAbsolute(cycles, memory);
                Byte operand = ReadByte(cycles, addr, memory);
                ADC(operand);
            } break;
            case INS_ADC_ABSX: {
                Word addr = AddrAbsoluteXY(cycles, X, memory);
                Byte operand = ReadByte(cycles, addr, memory);
                ADC(operand);
            } break;
            case INS_ADC_ABSY: {
                Word addr = AddrAbsoluteXY(cycles, Y, memory);
                Byte operand = ReadByte(cycles, addr, memory);
                ADC(operand);
            } break;
            case INS_ADC_INDX: {
                Word addr = AddrIndirectX(cycles, memory);
                Byte operand = ReadByte(cycles, addr, memory);
                ADC(operand);
            } break;
            case INS_ADC_INDY: {
                Word addr = AddrIndirectY(cycles, memory);
                Byte operand = ReadByte(cycles, addr, memory);
                ADC(operand);
            } break;
            case INS_CMP_IM: {
                Byte operand = FetchByte(cycles, memory);
                CMP(operand);
            } break;
            case INS_CMP_ZP: {
                Word addr = AddrZeroPage(cycles, memory);
                Byte operand = ReadByte(cycles, addr, memory);
                CMP(operand);
            } break;
            case INS_CMP_ZPX: {
                Word addr = AddrZeroPageXY(cycles, X, memory);
                Byte operand = ReadByte(cycles, addr, memory);
                CMP(operand);
            } break;
            case INS_CMP_ABS: {
                Word addr = AddrAbsolute(cycles, memory);
                Byte operand = ReadByte(cycles, addr, memory);
                CMP(operand);
            } break;
            case INS_CMP_ABSX: {
                Word addr = AddrAbsoluteXY(cycles, X, memory);
                Byte operand = ReadByte(cycles, addr, memory);
                CMP(operand);
            } break;
            case INS_CMP_ABSY: {
                Word addr = AddrAbsoluteXY(cycles, Y, memory);
                Byte operand = ReadByte(cycles, addr, memory);
                CMP(operand);
            } break;
            case INS_CMP_INDX: {
                Word addr = AddrIndirectX(cycles, memory);
                Byte operand = ReadByte(cycles, addr, memory);
                CMP(operand);
            } break;
            case INS_CMP_INDY: {
                Word addr = AddrIndirectY(cycles, memory);
                Byte operand = ReadByte(cycles, addr, memory);
                CMP(operand);
            } break;

            default: {
                throw "Instruction not implemented";
            } break;
        }
    }
    return cyclesRequested - cycles;
}

} // namespace cp6502
