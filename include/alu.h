#ifndef ALU_H
#define ALU_H

#include <systemc.h>

SC_MODULE(ALU) {
    // Inputs: Operands and 2-bit selector
    sc_in<sc_uint<8>> a;
    sc_in<sc_uint<8>> b;
    sc_in<sc_uint<2>> op; // 00: ADD, 01: SUB, 10: AND, 11: OR

    // Outputs: Result and Zero Flag
    sc_out<sc_uint<8>> result;
    sc_out<bool> zero_flag;
    sc_out<bool> carry_flag;
    sc_out<bool> negative_flag; // New: Bit 7 of result
    sc_out<bool> overflow_flag; // New: Signed overflow

    // Behavioral implementation of the ALU
    void execute() {
        sc_uint<8> val_a = a.read();
        sc_uint<8> val_b = b.read();
        sc_uint<2> opcode = op.read();

        // ----------------------------------------------------
        // PARALLEL EXECUTION (All components calculate at once)
        // ----------------------------------------------------

        // 1. Conditional Inversion (XOR) for Subtraction
        // Si opcode == 1 (SUB), xor_mask es 11111111 (255). Si no, es 00000000.
        sc_uint<8> xor_mask = (opcode == 1) ? 255 : 0; 
        sc_uint<8> b_mod = val_b ^ xor_mask;
        
        // El Cin es 1 solo en caso de resta para completar el Complemento a 2
        bool cin = (opcode == 1) ? 1 : 0;
        
        // Arithmetic Unit (using 9 bits to capture Carry Out)
        sc_uint<9> math_res_9 = (sc_uint<9>)val_a + (sc_uint<9>)b_mod + (sc_uint<9>)cin;
        sc_uint<8> math_res = math_res_9.range(7, 0);
        bool carry = math_res_9[8];
        carry_flag.write(carry);

        // 2. Logical Units
        sc_uint<8> and_res = val_a & val_b;
        sc_uint<8> or_res  = val_a | val_b;

        // ----------------------------------------------------
        // 4-to-1 MULTIPLEXER (MUX)
        // ----------------------------------------------------
        sc_uint<8> final_result;

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
        // NOR bit a bit para 8 bits
        bool z = !(final_result[7] | final_result[6] | final_result[5] | final_result[4] | 
                   final_result[3] | final_result[2] | final_result[1] | final_result[0]);

        // Output routing
        result.write(final_result);
        zero_flag.write(z);
        negative_flag.write((bool)final_result[7]);

        // Signed Overflow logic: (A7 == Bmod7) && (A7 != R7)
        // If adding two numbers of same sign results in different sign
        bool a7 = val_a[7];
        bool b7 = b_mod[7];
        bool r7 = final_result[7];
        bool v = (a7 == b7) && (a7 != r7);
        overflow_flag.write(v);
    }

    // Constructor: Defining the sensitivity list
    SC_CTOR(ALU) {
        SC_METHOD(execute);
        // Sensible a CUALQUIER cambio en las entradas de datos o el selector
        sensitive << a << b << op;
    }
};

#endif // ALU_H