#ifndef CONTROL_UNIT_H
#define CONTROL_UNIT_H

#include <systemc.h>
#include "isa.h"

// The ControlUnit is now purely combinatorial (Decodes instructions at the ID stage).
// No longer a multi-cycle state machine!
SC_MODULE(ControlUnit) {
    sc_in<sc_uint<16>> opcode;
    sc_in<sc_uint<16>> operand; // Needed for Rd, Rs decoding (now 16-bit)
    
    // Input flags for branch resolution at ID stage
    sc_in<bool> flag_z;
    sc_in<bool> flag_c;
    sc_in<bool> flag_n; // New input for Negative Flag
    sc_in<bool> flag_o; // New input for Overflow Flag

    // Control Signals for GPR Register File
    sc_out<bool> rf_we;
    sc_out<sc_uint<2>> rf_idx_w;
    sc_out<sc_uint<2>> rf_idx_r1;
    sc_out<sc_uint<2>> rf_idx_r2;
    sc_out<bool> rf_out_en; // Enable RF data to ALU
    
    // EX Stage controls
    sc_out<sc_uint<2>> alu_op;
    sc_out<bool> alu_en; // Enable ALU computation
    
    // MEM Stage controls
    sc_out<bool> ram_we;
    sc_out<bool> is_mem_access; 
    sc_out<bool> is_store; 

    // WB Stage controls
    sc_out<bool> out_load;
    sc_out<bool> is_indirect; // New: Signal for EX stage to use register as address

    // IF Stage branch control
    sc_out<bool> pc_load;

    void decode_logic() {
        // Defaults
        rf_we.write(0); rf_idx_w.write(0); rf_idx_r1.write(0); rf_idx_r2.write(0);
        rf_out_en.write(0); alu_op.write(0); alu_en.write(0);
        ram_we.write(0); is_mem_access.write(0); is_store.write(0);
        out_load.write(0); pc_load.write(0); is_indirect.write(0);

        sc_uint<8> op = opcode.read().to_uint();
        sc_uint<16> opr = operand.read().to_uint();

        // 1. Manejo de LOAD de Memoria (LD Rn, [Addr])
        if ((op & 0xFC) == 0x10) { 
            rf_idx_w.write(op & 0x03); 
            rf_we.write(1);
            is_mem_access.write(1);
        }
        // 2. Manejo de LDI (LDI Rn, Imm)
        else if ((op & 0xFC) == 0x20) {
            rf_idx_w.write(op & 0x03);
            rf_we.write(1);
            // LDI values come from operand, processed in WB stage via bus
        }
        // 3. Manejo de STORE (ST [Addr], Rn)
        else if ((op & 0xFC) == 0x30) {
            rf_idx_r1.write(op & 0x03); // Read register into bus
            rf_out_en.write(1);
            ram_we.write(1);
            is_store.write(1);
        }
        // 4. Manejo de LOAD INDIRECTO (LD Rd, [Rs])
        else if ((op & 0xFC) == 0x40) {
            rf_idx_w.write(op & 0x03);   // Rd is destination
            rf_idx_r2.write(opr & 0x03); // Rs is address source
            rf_we.write(1);
            is_mem_access.write(1);
            is_indirect.write(1);
        }
        // 5. Manejo de STORE INDIRECTO (ST [Rd], Rs)
        else if ((op & 0xFC) == 0x50) {
            rf_idx_r1.write(opr & 0x03); // Rs is data source
            rf_idx_r2.write(op & 0x03);  // Rd is address source
            rf_out_en.write(1);
            ram_we.write(1);
            is_store.write(1);
            is_indirect.write(1);
        }
        // 6. Aritmética Registro-Registro (ADD Rd, Rs / SUB Rd, Rs / MOV Rd, Rs)
        else if (op == OP_ADD_RR || op == OP_SUB_RR || op == OP_MOV) {
            rf_idx_w.write((opr >> 4) & 0x03);  // Rd (Destination)
            rf_we.write(1);
            rf_out_en.write(1);
            // Correction: For MOV Rd, Rs, we read Rs into R1 and then write R1 to Rd in WB
            if (op == OP_MOV) {
                rf_idx_r1.write(opr & 0x03);        // Rs is the source
            } else {
                rf_idx_r1.write((opr >> 4) & 0x03); // Rd is source1
                rf_idx_r2.write(opr & 0x03);        // Rs is source2
                alu_en.write(1);
                alu_op.write((op == OP_ADD_RR) ? 0 : 1);
            }
        }
        // 7. Control de Saltos
        else if (op == OP_JMP) {
            pc_load.write(1);
        } else if (op == OP_JZ) {
            if (flag_z.read() == true) pc_load.write(1);
        } else if (op == OP_JC) {
            if (flag_c.read() == true) pc_load.write(1);
        } else if (op == OP_JN) {
            if (flag_n.read() == true) pc_load.write(1);
        }
        // 6. I/O (OUT Rn)
        else if ((op & 0xFC) == 0xE0) {
            rf_idx_r1.write(op & 0x03);
            rf_out_en.write(1);
            out_load.write(1);
        }
        // 7. Halt
        else if (op == OP_HLT) {
            // No action needed here, handled by runner
        }
    }

    SC_CTOR(ControlUnit) {
        SC_METHOD(decode_logic);
        sensitive << opcode << operand << flag_z << flag_c << flag_n << flag_o;
    }
};

#endif // CONTROL_UNIT_H