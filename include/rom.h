#ifndef ROM_H
#define ROM_H

#include <systemc.h>

SC_MODULE(ROM) {
    sc_in<sc_uint<16>> addr;        // Address from PC (16-bit)
    sc_out<sc_uint<16>> data_out;   // 16-bit instruction output (Opcode + Operand)

    sc_uint<16> memory[256];
 
    void read_data() {
        uint16_t idx = addr.read().to_uint() & 0xFF;
        data_out.write(memory[idx]);
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
