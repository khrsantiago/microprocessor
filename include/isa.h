/*
Familia | Mnemónico | OpCode (Binario) | OpCode (Hex) |     Operando     | Descripción (Contrato Lógico)
──────────────────────────────────────────────────────────────────────────────────────────────────
Control |    NOP    |       0000       |      0x0     |     Ignorado     | No hace nada (No Operation). Útil para inicializar la RAM.
Memoria |    LDA    |       0001       |      0x1     | Dirección (0-15) | Load A: Lee la RAM en la [Dirección] y la guarda en el Reg A.
    ALU |    ADD    |       0010       |      0x2     | Dirección (0-15) | Suma: Suma el valor de la RAM en la [Dirección] al Reg A.
    ALU |    SUB    |       0011       |      0x3     | Dirección (0-15) | Resta: Resta el valor de la RAM en la [Dirección] al Reg A.
    I/O |    OUT    |       1110       |      0xE     |     Ignorado     | Salida: Envía el valor actual del Reg A al exterior.
Control |    HLT    |       1111       |      0xF     |     Ignorado     | Halt: Detiene el reloj del procesador. Fin del programa.
*/

#ifndef ISA_H
#define ISA_H

// Includes de SystemC no son estrictamente necesarios para enums estandar, 
// pero mantienen la arquitectura del archivo consistente
#include <systemc.h>

/**
 * @brief Definiciones de la Arquitectura del Conjunto de Instrucciones (ISA)
 * Formato de Instrucción de 16 bits (Dual-fetch): [OpCode: 8-bit] [Operand: 8-bit]
 */
enum OpCode {
    OP_NOP = 0x0, // 0000 - No Operation
    OP_LDA = 0x1, // 0001 - Load Accumulator
    OP_ADD = 0x2, // 0010 - Add to Accumulator
    OP_SUB = 0x3, // 0011 - Subtract from Accumulator
    OP_LDI = 0x4, // 0100 - Load Immediate
    OP_STA = 0x5, // 0101 - Store Accumulator to RAM
    OP_OUT = 0xE, // 1110 - Output Accumulator
    OP_JMP = 0x8, // 1000: Jump (Salto incondicional)
    OP_JZ  = 0x9, // 1001: Salto Condicional (Jump if Zero)
    OP_JC  = 0xA, // 1010: Salto Condicional (Jump if Carry)
    OP_HLT = 0xF  // 1111 - Halt Processor
};

// En arquitectura de 16 bits discreta no necesitamos mascaras para splittear
// El Opcode va en un byte entero, el Operando en el siguiente.

#endif // ISA_H