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
            {"LDA", OP_LD_R0}, {"STA", OP_ST_R0}, {"MOV", OP_MOV}
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
                if (c == ';' || c == '#') break;
                if (c == ',' || c == '[' || c == ']') continue;
                if (std::isalnum(c) || c == 'x' || c == 'X' || c == '-' || c == '_') res += (char)std::toupper(c);
            }
            return res;
        };

        auto is_num = [](const std::string& s) {
            if (s.empty()) return false;
            if (s[0] == '-') return s.size() > 1 && std::isdigit(s[1]);
            return std::isdigit(s[0]) || (s.size() > 1 && s[0] == '0' && std::toupper(s[1]) == 'X');
        };

        auto to_num = [](const std::string& s) {
            if (s.empty()) return 0UL;
            if (s.find("0X") == 0) return std::stoul(s, nullptr, 16);
            return (unsigned long)std::stol(s, nullptr, 10);
        };

        std::vector<std::string> lines;
        std::string raw_line;
        while (std::getline(file, raw_line)) lines.push_back(raw_line);

        std::map<std::string, int> labels;
        int current_addr = 0;

        // Pass 1: Collect Labels
        for (auto& line : lines) {
            size_t comment_pos = line.find(';');
            std::string work = (comment_pos != std::string::npos) ? line.substr(0, comment_pos) : line;
            work = trim(work);
            if (work.empty()) continue;

            size_t colon_pos = work.find(':');
            if (colon_pos != std::string::npos) {
                std::string label = clean(trim(work.substr(0, colon_pos)));
                if (is_num(label)) {
                    current_addr = (int)to_num(label);
                } else {
                    labels[label] = current_addr;
                }
                std::string rest = trim(work.substr(colon_pos + 1));
                if (rest.empty()) continue;
                work = rest;
            }

            std::stringstream ss(work);
            std::string mnemonic; ss >> mnemonic;
            mnemonic = clean(mnemonic);
            if (opcodes.count(mnemonic)) current_addr += 2;
            else if (is_num(mnemonic)) current_addr += 1;
        }

        // Pass 2: Assemble
        current_addr = 0;
        for (auto& line : lines) {
            size_t comment_pos = line.find(';');
            std::string work = (comment_pos != std::string::npos) ? line.substr(0, comment_pos) : line;
            work = trim(work);
            if (work.empty()) continue;

            size_t colon_pos = work.find(':');
            if (colon_pos != std::string::npos) {
                std::string label = clean(trim(work.substr(0, colon_pos)));
                if (is_num(label)) current_addr = (int)to_num(label);
                work = trim(work.substr(colon_pos + 1));
                if (work.empty()) continue;
            }

            std::stringstream ss(work);
            std::string mnemonic, t1, t2;
            if (!(ss >> mnemonic)) continue;
            mnemonic = clean(mnemonic);

            if (opcodes.count(mnemonic)) {
                unsigned int op = opcodes[mnemonic];
                unsigned int operand = 0;
                
                if (mnemonic == "LD" || mnemonic == "LDI" || mnemonic == "OUT" || mnemonic == "LDA") {
                    if (ss >> t1) {
                        t1 = clean(t1); int r = get_reg(t1);
                        if (r != -1) {
                            if (mnemonic == "LD" || mnemonic == "LDA") {
                                op = OP_LD_R0 + r;
                                if (ss >> t2) {
                                    t2 = clean(t2); int r_src = get_reg(t2);
                                    if (r_src != -1) { op = OP_LD_IND + r; operand = r_src; }
                                    else if (labels.count(t2)) operand = labels[t2];
                                    else if (is_num(t2)) operand = to_num(t2);
                                }
                            } else if (mnemonic == "LDI") {
                                op = OP_LDI_R0 + r;
                                if (ss >> t2) {
                                    t2 = clean(t2);
                                    if (labels.count(t2)) operand = labels[t2];
                                    else if (is_num(t2)) operand = to_num(t2);
                                }
                            } else if (mnemonic == "OUT") op = OP_OUT_R0 + r;
                        } else {
                            if (labels.count(t1)) operand = labels[t1];
                            else if (is_num(t1)) operand = to_num(t1);
                        }
                    }
                } else if (mnemonic == "ST" || mnemonic == "STA") {
                    if (ss >> t1 >> t2) {
                        t1 = clean(t1); t2 = clean(t2);
                        int r1 = get_reg(t1), r2 = get_reg(t2);
                        if (r1 != -1 && r2 != -1) { op = OP_ST_IND + r1; operand = r2; }
                        else if (is_num(t1) && r2 != -1) { op = OP_ST_R0 + r2; operand = to_num(t1); }
                        else if (labels.count(t1) && r2 != -1) { op = OP_ST_R0 + r2; operand = labels[t1]; }
                        else if (r1 != -1 && is_num(t2)) { op = OP_ST_R0 + r1; operand = to_num(t2); }
                        else if (r1 != -1 && labels.count(t2)) { op = OP_ST_R0 + r1; operand = labels[t2]; }
                    }
                } else if (mnemonic == "ADD" || mnemonic == "SUB" || mnemonic == "MOV") {
                    if (ss >> t1 >> t2) {
                        t1 = clean(t1); t2 = clean(t2);
                        int rd = get_reg(t1), rs = get_reg(t2);
                        if (mnemonic == "MOV") op = OP_MOV;
                        if (rd != -1 && rs != -1) operand = (rd << 4) | (rs & 0x0F);
                    }
                } else if (mnemonic == "JMP" || mnemonic == "JZ" || mnemonic == "JC" || mnemonic == "JN") {
                    if (ss >> t1) {
                        t1 = clean(t1);
                        if (labels.count(t1)) operand = labels[t1];
                        else if (is_num(t1)) operand = to_num(t1);
                    }
                }
                if (current_addr < mem_size - 1) {
                    memory[current_addr] = op & 0xFF; memory[current_addr + 1] = operand & 0xFF;
                    current_addr += 2;
                }
            } else if (is_num(mnemonic)) {
                if (current_addr < mem_size) memory[current_addr++] = to_num(mnemonic) & 0xFF;
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
