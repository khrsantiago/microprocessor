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
            {OP_LDA, "LDA"},
            {OP_ADD, "ADD"},
            {OP_SUB, "SUB"},
            {OP_LDI, "LDI"},
            {OP_STA, "STA"},
            {OP_TAX, "TAX"},
            {OP_TXA, "TXA"},
            {OP_LDA_X, "LDA_X"},
            {OP_STA_X, "STA_X"},
            {OP_JMP, "JMP"},
            {OP_JZ,  "JZ"},
            {OP_JC,  "JC"},
            {OP_OUT, "OUT"},
            {OP_HLT, "HLT"}
        };
        
        if (mnemonics.count(opcode)) return mnemonics[opcode];
        return "UNKNOWN";
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
