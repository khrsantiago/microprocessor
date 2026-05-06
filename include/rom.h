#ifndef ROM_H
#define ROM_H

#include <systemc.h>

SC_MODULE(ROM) {
    sc_in<sc_uint<8>> addr;         // Address from PC (8 bits)
    sc_out<sc_uint<16>> data_out;   // 16-bit instruction output (Opcode + Operand)

    sc_uint<8> memory[256];

    void read_data() {
        // Since instructions are 2 bytes (Opcode at addr, Operand at addr+1)
        sc_uint<8> byte1 = memory[addr.read()];
        sc_uint<8> byte2 = memory[(addr.read() + 1) % 256];
        
        // Assemble 16-bit word
        sc_uint<16> inst = (byte1 << 8) | byte2;
        data_out.write(inst);
    }

    SC_CTOR(ROM) {
        for (int i = 0; i < 256; i++) {
            memory[i] = 0;
        }

        SC_METHOD(read_data);
        sensitive << addr; 
    }
};

#endif // ROM_H
