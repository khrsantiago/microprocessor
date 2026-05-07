#ifndef ISA_H
#define ISA_H

// Includes de SystemC no son estrictamente necesarios para enums estandar, 
// pero mantienen la arquitectura del archivo consistente
#include <systemc.h>

/**
 * @brief Definiciones de la Arquitectura del Conjunto de Instrucciones (ISA)
 * Formato de Instrucción de 16 bits (Dual-fetch): [OpCode: 8 bits] [Operand: 8 bits]
 */
enum OpCode {
    OP_NOP = 0x00, // No Operation
    
    // Load/Store (Dest/Source specified in low 2 bits of opcode)
    OP_LD_R0 = 0x10, OP_LD_R1 = 0x11, OP_LD_R2 = 0x12, OP_LD_R3 = 0x13, // LD Rd, [Addr]
    OP_LDI_R0 = 0x20, OP_LDI_R1 = 0x21, OP_LDI_R2 = 0x22, OP_LDI_R3 = 0x23, // LDI Rd, Imm
    OP_ST_R0 = 0x30, OP_ST_R1 = 0x31, OP_ST_R2 = 0x32, OP_ST_R3 = 0x33, // ST [Addr], Rs

    // Register-Register Arithmetic (Rd and Rs encoded in operand byte)
    OP_ADD_RR = 0x60, // ADD Rd, Rs -> Operand: [Rd:4][Rs:4]
    OP_MOV    = 0x61, // MOV Rd, Rs -> Operand: [Rd:4][Rs:4]
    OP_SUB_RR = 0x70, // SUB Rd, Rs -> Operand: [Rd:4][Rs:4]

    // Control & IO
    OP_JMP = 0x80, // JMP Addr
    OP_JZ  = 0x90, // JZ Addr
    OP_JC  = 0xA0, // JC Addr
    OP_JN  = 0xB0, // JN Addr (Jump if Negative)
    
    // Indirect Memory (Rd and Rs encoded like LD/ST but uses different prefix)
    OP_LD_IND = 0x40, // LD Rd, [Rs] -> Low 2 bits of opcode: Rd, Operand: Rs
    OP_ST_IND = 0x50, // ST [Rd], Rs -> Low 2 bits of opcode: Rd, Operand: Rs

    OP_OUT_R0 = 0xE0, OP_OUT_R1 = 0xE1, OP_OUT_R2 = 0xE2, OP_OUT_R3 = 0xE3, // OUT Rs
    OP_HLT = 0xFF  // Halt
};

// Helpers for register indexing
#define REG_R0 0
#define REG_R1 1
#define REG_R2 2
#define REG_R3 3

// En arquitectura de 16 bits discreta no necesitamos mascaras para splittear
// El OpCode va en el primer byte, el Operando en el siguiente.

#endif // ISA_H