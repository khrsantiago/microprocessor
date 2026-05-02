#ifndef ALU_H
#define ALU_H

#include <systemc.h>

SC_MODULE(ALU) {
    // Inputs: Operands and 2-bit selector
    sc_in<sc_uint<4>> a;
    sc_in<sc_uint<4>> b;
    sc_in<sc_uint<2>> op; // 00: ADD, 01: SUB, 10: AND, 11: OR

    // Outputs: Result and Zero Flag
    sc_out<sc_uint<4>> result;
    sc_out<bool> zero_flag;

    // Behavioral implementation of the ALU
    void execute() {
        sc_uint<4> val_a = a.read();
        sc_uint<4> val_b = b.read();
        sc_uint<2> opcode = op.read();

        // ----------------------------------------------------
        // PARALLEL EXECUTION (All components calculate at once)
        // ----------------------------------------------------

        // 1. Conditional Inversion (XOR) for Subtraction
        // Si opcode == 1 (SUB), xor_mask es 1111. Si no, es 0000.
        sc_uint<4> xor_mask = (opcode == 1) ? 15 : 0; 
        sc_uint<4> b_mod = val_b ^ xor_mask;
        
        // El Cin es 1 solo en caso de resta para completar el Complemento a 2
        bool cin = (opcode == 1) ? 1 : 0;
        
        // Aritmetic Unit
        sc_uint<4> math_res = val_a + b_mod + cin;

        // Logical Unit
        sc_uint<4> and_res = val_a & val_b;
        sc_uint<4> or_res  = val_a | val_b;

        // ----------------------------------------------------
        // 4-to-1 MULTIPLEXER (MUX)
        // ----------------------------------------------------
        sc_uint<4> final_result;

        switch(opcode) {
            case 0: final_result = math_res; break; // ADD
            case 1: final_result = math_res; break; // SUB
            case 2: final_result = and_res;  break; // AND
            case 3: final_result = or_res;   break; // OR
            default: final_result = 0;       break; // Falla de seguridad
        }

        // ----------------------------------------------------
        // ZERO FLAG (Explicit NOR gate logic)
        // ----------------------------------------------------
        // NOR bit a bit: Si algun bit es 1, el OR da 1, y el NOT lo vuelve 0.
        // Solo si todos son 0, el OR da 0, y el NOT lo vuelve 1 (Zero encendido).
        bool z = !(final_result[3] | final_result[2] | final_result[1] | final_result[0]);

        // Output routing
        result.write(final_result);
        zero_flag.write(z);
    }

    // Constructor: Defining the sensitivity list
    SC_CTOR(ALU) {
        SC_METHOD(execute);
        // Sensible a CUALQUIER cambio en las entradas de datos o el selector
        sensitive << a << b << op;
    }
};

#endif // ALU_H