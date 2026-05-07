#ifndef REGISTER_FILE_H
#define REGISTER_FILE_H

#include <systemc.h>

/**
 * @brief Register File (GPR) for KHR-16.
 * Contains 4 x 16-bit registers (R0, R1, R2, R3).
 * Dual-read ports, Single-write port.
 */
SC_MODULE(RegisterFile) {
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<bool> we;                // Write Enable
    sc_in<sc_uint<2>> idx_w;      // Destination Register Index (Write)
    sc_in<sc_uint<2>> idx_r1;     // Source Register 1 Index (Read)
    sc_in<sc_uint<2>> idx_r2;     // Source Register 2 Index (Read)
    sc_in<sc_uint<16>> data_in;     // Data to write
 
    sc_out<sc_uint<16>> data_out1;  // Output for Source 1
    sc_out<sc_uint<16>> data_out2;  // Output for Source 2
 
    // Debug ports
    sc_out<sc_uint<16>> r0_out;
    sc_out<sc_uint<16>> r1_out;
    sc_out<sc_uint<16>> r2_out;
    sc_out<sc_uint<16>> r3_out;
 
    sc_uint<16> regs[4];

    void register_logic() {
        if (reset.read() == 1) {
            for(int i = 0; i < 4; i++) regs[i] = 0;
        } else if (clk.posedge()) {
            if (we.read() == 1) {
                regs[idx_w.read()] = data_in.read();
            }
        }
        
        // Continuous read (combinatorial or clocked? usually RF read is combinatorial)
        data_out1.write(regs[idx_r1.read()]);
        data_out2.write(regs[idx_r2.read()]);

        // Debug outputs
        r0_out.write(regs[0]);
        r1_out.write(regs[1]);
        r2_out.write(regs[2]);
        r3_out.write(regs[3]);
    }

    SC_CTOR(RegisterFile) {
        for(int i = 0; i < 4; i++) regs[i] = 0;

        SC_METHOD(register_logic);
        sensitive << clk.pos() << reset << idx_r1 << idx_r2;
    }
};

#endif
