#ifndef INSTRUCTION_REGISTER_H
#define INSTRUCTION_REGISTER_H

#include <systemc.h>

// Registro de Instrucciones (IR) - 16 bits (Dual-stage load)
SC_MODULE(InstructionRegister) {
    sc_in<bool> clk;              // Reloj del sistema
    sc_in<bool> ld_opcode;        // Carga el primer byte (OpCode)
    sc_in<bool> ld_operand;       // Carga el segundo byte (Operando)
    sc_in<sc_lv<8>> data_in;      // Entrada de 8 bits desde el Bus Central
    
    // Salidas (ahora de 8 bits cada una)
    sc_out<sc_lv<8>> opcode_out;  
    sc_out<sc_lv<8>> operand_out; 

    // Memoria interna del registro
    sc_lv<8> internal_opcode;
    sc_lv<8> internal_operand;

    void update_state() {
        if (clk.posedge()) {
            if (ld_opcode.read() == 1) {
                internal_opcode = data_in.read();
            }
            if (ld_operand.read() == 1) {
                internal_operand = data_in.read();
            }
            
            opcode_out.write(internal_opcode);
            operand_out.write(internal_operand);
        }
    }

    SC_CTOR(InstructionRegister) {
        // Inicializamos
        internal_opcode = "00000000";
        internal_operand = "00000000";
        
        SC_METHOD(update_state);
        sensitive << clk.pos();
    }
};

#endif // INSTRUCTION_REGISTER_H