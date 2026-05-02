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
 * Formato de Instrucción de 8 bits: [OpCode: 4-bit] [Operand: 4-bit]
 */
enum OpCode {
    OP_NOP = 0x0, // 0000 - No Operation
    OP_LDA = 0x1, // 0001 - Load Accumulator
    OP_ADD = 0x2, // 0010 - Add to Accumulator
    OP_SUB = 0x3, // 0011 - Subtract from Accumulator
    OP_OUT = 0xE, // 1110 - Output Accumulator
    OP_JMP = 0x8, // 1000: Jump (Salto incondicional)
    OP_JZ  = 0x9, // 1001: Salto Condicional (Jump if Zero)
    OP_HLT = 0xF  // 1111 - Halt Processor
};

// Mascayara para extraer las partes de una instruccion de 8 bits
const unsigned int OPCODE_MASK  = 0xF0; // 11110000 (Extrae los 4 bits superiores)
const unsigned int OPERAND_MASK = 0x0F; // 00001111 (Extrae los 4 bits inferiores)

#endif // ISA_H