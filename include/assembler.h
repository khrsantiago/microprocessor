#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include "isa.h"

class AssemblerLoader {
public:
    static bool load(const std::string& filename, sc_uint<8>* memory, int mem_size = 256) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: No se pudo abrir el archivo " << filename << std::endl;
            return false;
        }

        std::map<std::string, OpCode> opcodes = {
            {"NOP", OP_NOP}, {"HLT", OP_HLT}, {"JMP", OP_JMP},
            {"JZ", OP_JZ},   {"JC", OP_JC}, {"JN", OP_JN},
            {"LD", OP_LD_R0}, {"LDI", OP_LDI_R0}, {"ST", OP_ST_R0},
            {"ADD", OP_ADD_RR}, {"SUB", OP_SUB_RR}, {"OUT", OP_OUT_R0},
            {"LDA", OP_LD_R0}, {"STA", OP_ST_R0}
        };

        auto get_reg = [](std::string r) -> int {
            if (r == "R0") return 0;
            if (r == "R1") return 1;
            if (r == "R2") return 2;
            if (r == "R3") return 3;
            return -1;
        };

        auto clean = [](std::string s) {
            std::string res;
            for (char c : s) {
                if (c == ';' || c == '#' || c == ',') break;
                if (std::isalnum(c) || c == 'x' || c == 'X' || c == '-') res += (char)std::toupper(c);
            }
            return res;
        };

        auto is_num = [](const std::string& s) {
            if (s.empty()) return false;
            if (s[0] == '-') return s.size() > 1 && std::isdigit(s[1]);
            return std::isdigit(s[0]) || (s.size() > 1 && s[0] == '0' && std::toupper(s[1]) == 'X');
        };

        auto to_num = [](const std::string& s) {
            try {
                if (s.empty()) return 0UL;
                if (s.find("0X") == 0) return std::stoul(s, nullptr, 16);
                return (unsigned long)std::stol(s, nullptr, 10);
            } catch (...) { 
                std::cerr << "CRITICAL: Falla conversion stol/stoul para token: [" << s << "]\n";
                throw; 
            }
        };

        std::string line;
        int address = 0;

        while (std::getline(file, line)) {
            size_t comment_pos = line.find(';');
            if (comment_pos != std::string::npos) line = line.substr(0, comment_pos);
            
            line = trim(line);
            if (line.empty()) continue;

            // Manejar Direcciones Explicitadas (ej: "100: 7")
            size_t colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                std::string addr_str = clean(trim(line.substr(0, colon_pos)));
                std::string val_str = clean(trim(line.substr(colon_pos + 1)));
                if (is_num(addr_str) && is_num(val_str)) {
                    int target = (int)to_num(addr_str);
                    if (target >= 0 && target < mem_size) memory[target] = to_num(val_str) & 0xFF;
                }
                continue;
            }

            if (address >= mem_size) continue;

            std::stringstream ss(line);
            std::string mnemonic;
            if (!(ss >> mnemonic)) continue;
            mnemonic = clean(mnemonic);
            if (mnemonic.empty()) continue;

            if (opcodes.count(mnemonic)) {
                unsigned int op = opcodes[mnemonic];
                unsigned int operand = 0;
                std::string t1, t2;

                if (mnemonic == "LD" || mnemonic == "LDI" || mnemonic == "OUT" || mnemonic == "LDA") {
                    if (ss >> t1) {
                        t1 = clean(t1);
                        int r = get_reg(t1);
                        if (r != -1) {
                            if (mnemonic == "LD" || mnemonic == "LDA") op = OP_LD_R0 + r;
                            else if (mnemonic == "LDI") op = OP_LDI_R0 + r;
                            else if (mnemonic == "OUT") op = OP_OUT_R0 + r;
                            
                            if (mnemonic != "OUT" && ss >> t2) {
                                t2 = clean(t2);
                                if (is_num(t2)) operand = to_num(t2);
                            }
                        } else if (is_num(t1)) {
                            operand = to_num(t1);
                        }
                    }
                }
                else if (mnemonic == "ST" || mnemonic == "STA") {
                    if (ss >> t1 >> t2) {
                        t1 = clean(t1); t2 = clean(t2);
                        int r1 = get_reg(t1), r2 = get_reg(t2);
                        if (r1 != -1) { op = OP_ST_R0 + r1; if (is_num(t2)) operand = to_num(t2); }
                        else { if (r2 != -1) op = OP_ST_R0 + r2; if (is_num(t1)) operand = to_num(t1); }
                    }
                }
                else if (mnemonic == "ADD" || mnemonic == "SUB") {
                    if (ss >> t1 >> t2) {
                        t1 = clean(t1); t2 = clean(t2);
                        int rd = get_reg(t1), rs = get_reg(t2);
                        if (rd != -1 && rs != -1) operand = (rd << 4) | (rs & 0x0F);
                        else if (rd != -1) operand = (rd << 4);
                    }
                }
                else if (mnemonic == "JMP" || mnemonic == "JZ" || mnemonic == "JC" || mnemonic == "JN") {
                    if (ss >> t1) { t1 = clean(t1); if (is_num(t1)) operand = to_num(t1); }
                }

                memory[address] = op & 0xFF;
                memory[address + 1] = operand & 0xFF;
                address += 2;
            } else if (is_num(mnemonic)) {
                memory[address++] = to_num(mnemonic) & 0xFF;
            }
        }
        return true;
    }

private:
    static std::string trim(const std::string& s) {
        auto start = s.begin();
        while (start != s.end() && std::isspace(*start)) start++;
        auto end = s.end();
        do { end--; } while (std::distance(start, end) > 0 && std::isspace(*end));
        return (start >= end + 1) ? "" : std::string(start, end + 1);
    }
};

#endif
