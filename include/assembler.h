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

/**
 * @brief Simple C++ Assembler/Loader for the SAP-1 Microprocessor.
 * Directamente lee archivos .asm y los carga en la memoria del simulador.
 */
class AssemblerLoader {
public:
    static bool load(const std::string& filename, sc_uint<8>* memory, int mem_size = 256) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: No se pudo abrir el archivo " << filename << std::endl;
            return false;
        }

        std::map<std::string, OpCode> opcodes = {
            {"NOP", OP_NOP}, {"LDA", OP_LDA}, {"ADD", OP_ADD},
            {"SUB", OP_SUB}, {"OUT", OP_OUT}, {"JMP", OP_JMP},
            {"JZ", OP_JZ},   {"JC", OP_JC},   {"HLT", OP_HLT}, 
            {"LDI", OP_LDI}, {"STA", OP_STA},
            {"TAX", OP_TAX}, {"TXA", OP_TXA},
            {"LDA_X", OP_LDA_X}, {"STA_X", OP_STA_X}
        };

        std::string line;
        int address = 0;

        while (std::getline(file, line)) {
            // 1. Eliminar comentarios
            size_t comment_pos = line.find(';');
            if (comment_pos != std::string::npos) {
                line = line.substr(0, comment_pos);
            }

            // 2. Limpiar espacios y convertir a mayúsculas
            line = trim(line);
            if (line.empty()) continue;
            std::transform(line.begin(), line.end(), line.begin(), ::toupper);

            // 3. Manejar Direcciones Explicitadas (ej: "14: 7")
            size_t colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                std::string addr_str = trim(line.substr(0, colon_pos));
                std::string val_str = trim(line.substr(colon_pos + 1));
                
                try {
                    int target_addr = (addr_str.find("0X") == 0) ? std::stoul(addr_str, nullptr, 16) : std::stoul(addr_str, nullptr, 10);
                    if (target_addr >= 0 && target_addr < mem_size) {
                        if (!val_str.empty()) {
                            unsigned int val = (val_str.find("0X") == 0) ? std::stoul(val_str, nullptr, 16) : std::stoul(val_str, nullptr, 10);
                            memory[target_addr] = val & 0xFF;
                        }
                        // Si era una etiqueta de salto (ej: "LOOP: LDA 5"), actualizamos address
                        // Pero para esta ISA simplificada, asumimos que address sigue secuencial si no hay valor
                        // address = target_addr; // Opcional
                    }
                } catch (...) {}
                continue;
            }

            // 4. Parsear Token de instruccion secuencial
            if (address >= mem_size) continue;

            std::stringstream ss(line);
            std::string mnemonic;
            ss >> mnemonic;

            if (opcodes.count(mnemonic)) {
                OpCode op = opcodes[mnemonic];
                unsigned int operand = 0;

                if (op == OP_LDA || op == OP_ADD || op == OP_SUB || op == OP_JMP || op == OP_JZ || op == OP_JC || op == OP_LDI || op == OP_STA) {
                    std::string op_str;
                    if (ss >> op_str) {
                        try {
                            operand = (op_str.find("0X") == 0) ? std::stoul(op_str, nullptr, 16) : std::stoul(op_str, nullptr, 10);
                        } catch (...) {
                            std::cerr << "Error: Operando invalido en linea: " << line << std::endl;
                            return false;
                        }
                    }
                }

                // Cada instrucción ocupa 2 bytes: [OpCode] y [Operando]
                // Si la instrucción no usa operando, guardamos un 0x00 para mantener la estructura uniforme
                memory[address] = op;
                memory[address + 1] = operand & 0xFF;
                address += 2;
            } else {
                try {
                    unsigned int val = (mnemonic.find("0X") == 0) ? std::stoul(mnemonic, nullptr, 16) : std::stoul(mnemonic, nullptr, 10);
                    memory[address] = val & 0xFF;
                    address++;
                } catch (...) {
                    std::cerr << "Error: Token desconocido: " << mnemonic << std::endl;
                    return false;
                }
            }
        }

        std::cout << "Assembler: Cargadas " << address << " lineas en memoria." << std::endl;
        return true;
    }

private:
    static std::string trim(const std::string& s) {
        auto start = s.begin();
        while (start != s.end() && std::isspace(*start)) start++;
        auto end = s.end();
        do { end--; } while (std::distance(start, end) > 0 && std::isspace(*end));
        return std::string(start, end + 1);
    }
};

#endif
