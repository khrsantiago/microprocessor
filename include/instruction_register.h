#ifndef INSTRUCTION_REGISTER_H
#define INSTRUCTION_REGISTER_H

#include <systemc.h>

// Registro de Instrucciones (IR) - 8 bits
SC_MODULE(InstructionRegister) {
    sc_in<bool> clk;              // Reloj del sistema
    sc_in<bool> ld;               // Load Enable (Habilitador de carga)
    sc_in<sc_lv<8>> data_in;      // Entrada de 8 bits desde el Bus Central
    
    // Salidas bifurcadas (Bit-Slicing)
    sc_out<sc_lv<4>> opcode_out;  // Los 4 bits superiores van a la Unidad de Control
    sc_out<sc_lv<4>> operand_out; // Los 4 bits inferiores regresan al bus de direcciones

    // Memoria interna del registro
    sc_lv<8> internal_ir;

    void update_state() {
        if (clk.posedge()) {
            // Si Load Enable esta activo, atrapamos la instruccion del bus
            if (ld.read() == 1) {
                internal_ir = data_in.read();
            }
            
            // Decodificacion en tiempo real (Bit-Slicing)
            // Extraemos los bits [7,6,5,4] para el OpCode
            opcode_out.write(internal_ir.range(7, 4));
            
            // Extraemos los bits [3,2,1,0] para la Direccion Operando
            operand_out.write(internal_ir.range(3, 0));
        }
    }

    SC_CTOR(InstructionRegister) {
        // Inicializamos con NOP (00000000)
        internal_ir = "00000000";
        
        SC_METHOD(update_state);
        sensitive << clk.pos();
    }
};

#endif // INSTRUCTION_REGISTER_H