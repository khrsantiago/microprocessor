#ifndef PIPELINE_REGISTERS_H
#define PIPELINE_REGISTERS_H

#include <systemc.h>

/**
 * @brief IF/ID Pipeline Register
 * Separates Instruction Fetch from Instruction Decode
 */
SC_MODULE(IF_ID_Reg) {
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<bool> enable;
    sc_in<bool> flush;

    sc_in<sc_uint<16>> pc_in;
    sc_in<sc_lv<16>> opcode_in;
    sc_in<sc_lv<16>> operand_in;

    sc_out<sc_uint<16>> pc_out;
    sc_out<sc_lv<16>> opcode_out;
    sc_out<sc_uint<16>> operand_out; 

    sc_uint<16> pc_val;
    sc_lv<8> opcode_val;
    sc_lv<16> operand_val;

    void update() {
        if (reset.read()) {
            pc_val = 0;
            opcode_val = "00000000";
            operand_val = "00000000";
        } else if (flush.read()) {
            opcode_val = "00000000"; // NOP
            operand_val = 0;
        } else if (enable.read()) {
            pc_val = pc_in.read();
            opcode_val = opcode_in.read();
            operand_val = operand_in.read();
        }
        
        pc_out.write(pc_val);
        opcode_out.write(opcode_val);
        
        // Extension de signo selectiva (Solo LDI)
        sc_uint<16> extended_opr = 0;
        sc_uint<8> val8 = operand_val.to_uint();
        uint8_t op = opcode_val.to_uint();
        
        if (op >= 0x20 && op <= 0x23) { // OP_LDI_R0 to OP_LDI_R3
            if (val8[7] == 1) extended_opr = 0xFF00 | val8;
            else extended_opr = val8;
        } else {
            extended_opr = val8;
        }
        operand_out.write(extended_opr);
    }

    SC_CTOR(IF_ID_Reg) {
        SC_METHOD(update);
        sensitive << clk.pos();
        pc_val = 0;
        opcode_val = "00000000";
        operand_val = 0;
    }
};

/**
 * @brief ID/EX Pipeline Register
 * Separates Instruction Decode from Execution
 */
SC_MODULE(ID_EX_Reg) {
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<bool> enable;
    sc_in<bool> flush;

    // Control signals for Execute Stage
    sc_in<sc_uint<2>> alu_op_in;
    sc_in<bool> b_in_ctrl;

    // Control signals for WriteBack Stage
    sc_in<bool> rf_we_in;
    sc_in<sc_uint<2>> rf_idx_w_in;
    sc_in<sc_uint<2>> rf_idx_r1_in; // Added for forwarding
    sc_in<sc_uint<2>> rf_idx_r2_in; // Added for forwarding
    sc_out<sc_uint<2>> rf_idx_r1_out;
    sc_out<sc_uint<2>> rf_idx_r2_out;
    sc_in<bool> out_load_ctrl;
    sc_in<bool> ram_we_ctrl; // Memory Write
    sc_in<bool> is_indirect_in; // New

    // Data paths
    sc_in<sc_uint<16>> operand_in;
    sc_in<sc_uint<16>> rf_data1_in;
    sc_in<sc_uint<16>> rf_data2_in;
    
    // Outputs
    sc_out<sc_uint<2>> alu_op_out;
    sc_out<bool> b_in_out;
    sc_out<bool> rf_we_out;
    sc_out<sc_uint<2>> rf_idx_w_out;
    sc_out<bool> out_load_out;
    sc_out<bool> ram_we_out;
    sc_out<bool> is_indirect_out; // New
    
    sc_out<sc_uint<16>> operand_out;
    sc_out<sc_uint<16>> rf_data1_out;
    sc_out<sc_uint<16>> rf_data2_out;
    // To identify the instruction
    sc_in<sc_uint<16>> opcode_in;
    sc_out<sc_uint<16>> opcode_out;
    sc_in<sc_uint<16>> pc_in;
    sc_out<sc_uint<16>> pc_out;

    // Internal states
    sc_uint<2> alu_op_val;
    bool b_in_val, rf_we_val, out_load_val, ram_we_val, is_indirect_val;
    sc_uint<2> rf_idx_w_val, rf_idx_r1_val, rf_idx_r2_val;
    sc_uint<16> operand_val, rf_data1_val, rf_data2_val;
    sc_uint<16> opcode_val;
    sc_uint<16> pc_val;

    void update() {
        if (reset.read() || flush.read()) {
            alu_op_val = 0;
            b_in_val = 0; rf_we_val = 0; out_load_val = 0; ram_we_val = 0; rf_idx_w_val = 0;
            operand_val = 0; rf_data1_val = 0; rf_data2_val = 0; opcode_val = 0; is_indirect_val = 0;
        } else if (enable.read()) {
            alu_op_val = alu_op_in.read();
            b_in_val = b_in_ctrl.read();
            rf_we_val = rf_we_in.read();
            rf_idx_w_val = rf_idx_w_in.read();
            rf_idx_r1_val = rf_idx_r1_in.read();
            rf_idx_r2_val = rf_idx_r2_in.read();
            out_load_val = out_load_ctrl.read();
            ram_we_val = ram_we_ctrl.read();
            is_indirect_val = is_indirect_in.read();
            
            operand_val = operand_in.read();
            rf_data1_val = rf_data1_in.read();
            rf_data2_val = rf_data2_in.read();
            opcode_val = opcode_in.read();
            pc_val = pc_in.read();
        }

        alu_op_out.write(alu_op_val);
        b_in_out.write(b_in_val);
        rf_we_out.write(rf_we_val);
        rf_idx_w_out.write(rf_idx_w_val);
        rf_idx_r1_out.write(rf_idx_r1_val);
        rf_idx_r2_out.write(rf_idx_r2_val);
        out_load_out.write(out_load_val);
        ram_we_out.write(ram_we_val);
        is_indirect_out.write(is_indirect_val);

        operand_out.write(operand_val);
        rf_data1_out.write(rf_data1_val);
        rf_data2_out.write(rf_data2_val);
        opcode_out.write(opcode_val);
        pc_out.write(pc_val);
    }

    SC_CTOR(ID_EX_Reg) {
        SC_METHOD(update);
        sensitive << clk.pos();
        alu_op_val = 0;
        b_in_val = 0; rf_we_val = 0; out_load_val = 0; ram_we_val = 0; rf_idx_w_val = 0;
        operand_val = 0; rf_data1_val = 0; rf_data2_val = 0; opcode_val = 0; pc_val = 0; is_indirect_val = 0;
    }
};

/**
 * @brief EX/WB Pipeline Register
 * Separates Execute from WriteBack
 */
SC_MODULE(EX_WB_Reg) {
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<bool> enable;

    // Control signals for Writeback
    sc_in<bool> rf_we_ctrl; 
    sc_in<sc_uint<2>> rf_idx_w_in;
    sc_in<bool> out_load_ctrl;
    sc_in<bool> ram_we_ctrl; // Memory write

    // Data passed from EX
    sc_in<sc_uint<16>> alu_result_in;
    sc_in<sc_uint<16>> operand_in;
    sc_in<sc_uint<16>> opcode_in;
    sc_in<sc_uint<16>> rf_data1_in; // In case we need Rd value
    sc_in<sc_uint<16>> ram_data_in; // In case we fetched from RAM in EX

    sc_in<sc_uint<16>> pc_in;
    sc_out<sc_uint<16>> pc_out;

    sc_out<bool> rf_we_out;
    sc_out<sc_uint<2>> rf_idx_w_out;
    sc_out<bool> out_load_out;
    sc_out<bool> ram_we_out;

    sc_out<sc_uint<16>> alu_result_out;
    sc_out<sc_uint<16>> operand_out;
    sc_out<sc_uint<16>> opcode_out;
    sc_out<sc_uint<16>> rf_data1_out;
    sc_out<sc_uint<16>> ram_data_out;

    bool rf_we_val, out_load_val, ram_we_val;
    sc_uint<2> rf_idx_w_val;
    sc_uint<16> alu_result_val, operand_val, rf_data1_val, ram_data_val;
    sc_uint<16> opcode_val;
    sc_uint<16> pc_val;

    void update() {
        if (reset.read()) {
            rf_we_val = 0; out_load_val = 0; ram_we_val = 0; rf_idx_w_val = 0;
            alu_result_val = 0; operand_val = 0; opcode_val = 0; rf_data1_val = 0; ram_data_val = 0; pc_val = 0;
        } else if (enable.read()) {
            rf_we_val = rf_we_ctrl.read();
            rf_idx_w_val = rf_idx_w_in.read();
            out_load_val = out_load_ctrl.read();
            ram_we_val = ram_we_ctrl.read();
            
            alu_result_val = alu_result_in.read();
            operand_val = operand_in.read();
            opcode_val = opcode_in.read();
            rf_data1_val = rf_data1_in.read();
            ram_data_val = ram_data_in.read();
            pc_val = pc_in.read();
        }

        rf_we_out.write(rf_we_val);
        rf_idx_w_out.write(rf_idx_w_val);
        out_load_out.write(out_load_val);
        ram_we_out.write(ram_we_val);
        
        alu_result_out.write(alu_result_val);
        operand_out.write(operand_val);
        opcode_out.write(opcode_val);
        rf_data1_out.write(rf_data1_val);
        ram_data_out.write(ram_data_val);
        pc_out.write(pc_val);
    }

    SC_CTOR(EX_WB_Reg) {
        SC_METHOD(update);
        sensitive << clk.pos();
        rf_we_val = 0; out_load_val = 0; ram_we_val = 0; rf_idx_w_val = 0;
        alu_result_val = 0; operand_val = 0; opcode_val = 0; rf_data1_val = 0; ram_data_val = 0; pc_val = 0;
    }
};

#endif // PIPELINE_REGISTERS_H
