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
        
        // Continuous read with Write-Through logic (N-1 to N dependency in ID)
        sc_uint<2> r1 = idx_r1.read();
        sc_uint<2> r2 = idx_r2.read();
        sc_uint<16> out1 = regs[r1];
        sc_uint<16> out2 = regs[r2];

        // If currently writing to the same register we are reading
        if (we.read() == 1) {
            if (idx_w.read() == r1) out1 = data_in.read();
            if (idx_w.read() == r2) out2 = data_in.read();
        }

        data_out1.write(out1);
        data_out2.write(out2);

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
