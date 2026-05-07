#ifndef ALU_H
#define ALU_H

#include <systemc.h>

SC_MODULE(ALU) {
    // Inputs: Operands and 2-bit selector
    sc_in<sc_uint<16>> a;
    sc_in<sc_uint<16>> b;
    sc_in<sc_uint<2>> op; // 00: ADD, 01: SUB, 10: AND, 11: OR

    // Outputs: Result and Flags
    sc_out<sc_uint<16>> result;
    sc_out<bool> zero_flag;
    sc_out<bool> carry_flag;
    sc_out<bool> negative_flag; // Bit 15 of result
    sc_out<bool> overflow_flag; // Signed overflow (bit 15)

    // Behavioral implementation of the ALU
    void execute() {
        sc_uint<16> val_a = a.read();
        sc_uint<16> val_b = b.read();
        sc_uint<2> opcode = op.read();

        // ----------------------------------------------------
        // PARALLEL EXECUTION
        // ----------------------------------------------------

        // 1. Conditional Inversion (XOR) for Subtraction
        sc_uint<16> xor_mask = (opcode == 1) ? 0xFFFF : 0; 
        sc_uint<16> b_mod = val_b ^ xor_mask;
        
        bool cin = (opcode == 1) ? 1 : 0;
        
        // Arithmetic Unit (using 17 bits to capture Carry Out)
        sc_uint<17> math_res_17 = (sc_uint<17>)val_a + (sc_uint<17>)b_mod + (sc_uint<17>)cin;
        sc_uint<16> math_res = math_res_17.range(15, 0);
        bool carry = math_res_17[16];
        carry_flag.write(carry);

        // 2. Logical Units
        sc_uint<16> and_res = val_a & val_b;
        sc_uint<16> or_res  = val_a | val_b;

        // ----------------------------------------------------
        // 4-to-1 MULTIPLEXER (MUX)
        // ----------------------------------------------------
        sc_uint<16> final_result;

        switch(opcode) {
            case 0: final_result = math_res; break; // ADD
            case 1: final_result = math_res; break; // SUB
            case 2: final_result = and_res;  break; // AND
            case 3: final_result = or_res;   break; // OR
            default: final_result = 0;       break;
        }

        // ----------------------------------------------------
        // FLAGS LOGIC
        // ----------------------------------------------------
        
        // Zero Flag: True if result is 0
        bool z = (final_result == 0);

        // Output routing
        result.write(final_result);
        zero_flag.write(z);
        negative_flag.write((bool)final_result[15]);

        // Signed Overflow logic: (A15 == Bmod15) && (A15 != R15)
        bool a15 = val_a[15];
        bool b15 = b_mod[15];
        bool r15 = final_result[15];
        bool v = (a15 == b15) && (a15 != r15);
        overflow_flag.write(v);
    }

    SC_CTOR(ALU) {
        SC_METHOD(execute);
        sensitive << a << b << op;
    }
};

#endif // ALU_H