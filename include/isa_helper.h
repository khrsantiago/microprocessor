#ifndef ISA_HELPER_H
#define ISA_HELPER_H

#include <string>
#include <map>
#include "isa.h"

class IsAHelper {
public:
    static std::string get_mnemonic(unsigned char opcode) {
        static std::map<unsigned char, std::string> mnemonics = {
            {OP_NOP, "NOP"},
            {OP_LD_R0, "LD R0"}, {OP_LD_R1, "LD R1"}, {OP_LD_R2, "LD R2"}, {OP_LD_R3, "LD R3"},
            {OP_LDI_R0, "LDI R0"}, {OP_LDI_R1, "LDI R1"}, {OP_LDI_R2, "LDI R2"}, {OP_LDI_R3, "LDI R3"},
            {OP_ST_R0, "ST R0"}, {OP_ST_R1, "ST R1"}, {OP_ST_R2, "ST R2"}, {OP_ST_R3, "ST R3"},
            {OP_ADD_RR, "ADD"}, {OP_SUB_RR, "SUB"},
            {OP_JMP, "JMP"}, {OP_JZ, "JZ"}, {OP_JC, "JC"}, {OP_JN, "JN"},
            {OP_OUT_R0, "OUT R0"}, {OP_OUT_R1, "OUT R1"}, {OP_OUT_R2, "OUT R2"}, {OP_OUT_R3, "OUT R3"},
            {OP_HLT, "HLT"}
        };
        
        if (mnemonics.count(opcode)) return mnemonics[opcode];
        return "UNKNOWN";
    }

    static std::string get_instruction_text(unsigned char opcode, unsigned char operand) {
        std::string mnem = get_mnemonic(opcode);
        if (mnem == "UNKNOWN") return "---";

        // 1. Aritmética Registro-Registro (ADD Rd, Rs / SUB Rd, Rs)
        if (opcode == OP_ADD_RR || opcode == OP_SUB_RR) {
            return mnem + " R" + std::to_string((operand >> 4) & 0x3) + ", R" + std::to_string(operand & 0x3);
        }

        // 2. I/O (OUT Rn)
        if ((opcode & 0xFC) == 0xE0) {
            return "OUT R" + std::to_string(opcode & 0x03);
        }

        // 3. Carga Inmediata (LDI Rn, Imm) - Mostrar como signo para claridad
        if ((opcode & 0xFC) == 0x20) {
            signed char s_val = (signed char)operand;
            if (s_val < 0) {
                return mnem + ", " + std::to_string((int)s_val) + " (" + std::to_string((int)operand) + ")";
            }
            return mnem + ", " + std::to_string((int)operand);
        }

        // 4. Memoria (LD/ST [Addr], Rn)
        if ((opcode & 0xFC) == 0x10 || (opcode & 0xFC) == 0x30) {
            return mnem + ", [" + std::to_string((int)operand) + "]";
        }

        // 5. Saltos
        if (opcode == OP_JMP || opcode == OP_JZ || opcode == OP_JC || opcode == OP_JN) {
            return mnem + " " + std::to_string((int)operand);
        }

        return mnem;
    }

    // Colores ANSI
    static const char* RESET;
    static const char* RED;
    static const char* GREEN;
    static const char* BLUE;
    static const char* YELLOW;
    static const char* CYAN;
    static const char* BOLD;
};

const char* IsAHelper::RESET  = "\033[0m";
const char* IsAHelper::RED    = "\033[31m";
const char* IsAHelper::GREEN  = "\033[32m";
const char* IsAHelper::BLUE   = "\033[34m";
const char* IsAHelper::YELLOW = "\033[33m";
const char* IsAHelper::CYAN   = "\033[36m";
const char* IsAHelper::BOLD   = "\033[1m";

#endif
