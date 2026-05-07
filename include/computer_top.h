#ifndef COMPUTER_TOP_H
#define COMPUTER_TOP_H

#include <systemc.h>

#include "program_counter.h"
#include "rom.h"
#include "ram.h"
#include "register_file.h"
#include "alu.h"
#include "control_unit.h"
#include "output_register.h"
#include "pipeline_registers.h"
#include "vram.h"

SC_MODULE(ComputerTop) {
    sc_in<bool> clk;
    sc_in<bool> reset;

    // --- PIPELINE CONSTANTS ---
    sc_signal<bool> s_true;
    sc_signal<bool> s_false;

    // --- IF STAGE ---
    sc_signal<bool> s_pc_en;
    sc_signal<sc_uint<16>> s_pc_out; // PC is now 16-bit
    sc_signal<sc_uint<16>> s_rom_data;
    sc_signal<bool> s_if_flush;
    sc_signal<sc_lv<16>> s_rom_opcode;
    sc_signal<sc_lv<16>> s_rom_operand;

    // --- IF/ID REG ---
    sc_signal<sc_uint<16>> s_id_pc;
    sc_signal<sc_lv<16>> s_id_opcode_raw;
    sc_signal<sc_lv<16>> s_id_operand_raw;
    sc_signal<sc_uint<16>> s_id_opcode;
    sc_signal<sc_uint<16>> s_id_operand;
 
    // --- ID STAGE (GPR) ---
    sc_signal<sc_uint<2>> s_id_alu_op;
    sc_signal<bool> s_id_alu_en;
    sc_signal<bool> s_id_ram_we;
    sc_signal<bool> s_id_is_mem_access;
    sc_signal<bool> s_id_is_store;
    sc_signal<bool> s_id_out_load;
    sc_signal<bool> s_id_pc_load;
    sc_signal<bool> s_id_is_indirect; // New
 
    sc_signal<bool> s_id_rf_we;
    sc_signal<sc_uint<2>> s_id_rf_idx_w;
    sc_signal<sc_uint<2>> s_id_rf_idx_r1;
    sc_signal<sc_uint<2>> s_id_rf_idx_r2;
    sc_signal<bool> s_id_rf_out_en;
 
    sc_signal<sc_uint<16>> s_rf_data1;
    sc_signal<sc_uint<16>> s_rf_data1_fwd; // Forwarded to IdEx
    sc_signal<sc_uint<16>> s_rf_data2;
    sc_signal<sc_uint<16>> s_rf_data2_fwd; // Forwarded to IdEx
     
    // Debug RF ports
    sc_signal<sc_uint<16>> s_rf_r0, s_rf_r1, s_rf_r2, s_rf_r3;
    sc_signal<bool> s_flags_z, s_flags_c, s_flags_n, s_flags_o; 
    sc_signal<bool> s_id_fwd_z, s_id_fwd_c, s_id_fwd_n, s_id_fwd_o; // New: Forwarded flags for ID stage
    // --- ID/EX REG ---
    sc_signal<sc_uint<2>> s_ex_alu_op;
    sc_signal<bool> s_ex_alu_en;
    sc_signal<bool> s_ex_rf_we;
    sc_signal<sc_uint<2>> s_ex_rf_idx_w;
    sc_signal<bool> s_ex_out_load;
    sc_signal<bool> s_ex_ram_we;
    sc_signal<sc_uint<16>> s_ex_operand;
    sc_signal<sc_uint<16>> s_ex_rf_data1;
    sc_signal<sc_uint<16>> s_ex_rf_data2;
    sc_signal<bool> s_ex_is_indirect; // New
    sc_signal<sc_uint<16>> s_ex_opcode;
    sc_signal<sc_uint<2>> s_ex_rf_idx_r1;
    sc_signal<sc_uint<2>> s_ex_rf_idx_r2;
    sc_signal<bool> s_ex_is_mem_access;

    // --- EX STAGE ---
    sc_signal<sc_uint<16>> s_ex_rf_data1_fwd;
    sc_signal<sc_uint<16>> s_ex_rf_data2_fwd;
    sc_signal<sc_uint<16>> s_ex_alu_b_in;
    sc_signal<sc_uint<16>> s_ex_alu_result;
    sc_signal<bool> s_ex_alu_zero;
    sc_signal<bool> s_ex_alu_carry;
    sc_signal<bool> s_ex_alu_negative;
    sc_signal<bool> s_ex_alu_overflow;
    sc_signal<sc_uint<16>> s_ex_ram_addr; // Address is now 16-bit
    sc_signal<sc_uint<16>> s_ram_out;    // RAM chip output
    sc_signal<sc_uint<16>> s_vram_out;   // VRAM chip output
    sc_signal<sc_uint<16>> s_mem_data_mux; // Multiplexed memory output
    sc_signal<bool> s_ram_we_mux;
    sc_signal<bool> s_vram_we_mux;

    // --- EX/WB REG ---
    sc_signal<bool> s_wb_rf_we;
    sc_signal<sc_uint<2>> s_wb_rf_idx_w;
    sc_signal<bool> s_wb_out_load;
    sc_signal<bool> s_wb_ram_we;
    sc_signal<sc_uint<16>> s_wb_alu_result;
    sc_signal<sc_uint<16>> s_wb_operand;
    sc_signal<sc_uint<16>> s_wb_opcode;
    sc_signal<sc_uint<16>> s_wb_rf_data1;
    sc_signal<sc_uint<16>> s_wb_ram_data;

    // --- WB STAGE ---
    sc_signal<sc_uint<16>> s_wb_rf_data_in;
    sc_signal<sc_lv<16>> s_wb_out_data;
    sc_signal<sc_uint<16>> s_display_val;
    
    // Components
    ProgramCounter* Pc;
    ROM* InstRom;
    RAM* DataRam;
    VRAM* DataVram; // New VRAM module
    IF_ID_Reg* IfId;
    ID_EX_Reg* IdEx;
    EX_WB_Reg* ExWb;
    ControlUnit* Cu;
    RegisterFile* Rf;
    ALU* Alu;
    OutputRegister* OutReg;

    // Debug Traces
    sc_signal<sc_uint<16>> s_pc_to_bus;
    sc_signal<sc_lv<16>>   s_ir_opcode;
    sc_signal<sc_lv<16>>   s_ir_operand;
    sc_signal<sc_uint<16>> s_mar_to_ram; // Now 16-bit
    sc_signal<sc_uint<16>> s_ram_to_bus;
    sc_signal<sc_uint<16>> common_bus;
    
    sc_signal<sc_uint<16>> s_if_pc_dbg, s_id_pc_dbg, s_ex_pc_dbg, s_wb_pc_dbg;
    sc_signal<sc_uint<16>> s_if_opcode_dbg, s_if_operand_dbg;
    sc_signal<sc_uint<16>> s_ex_opcode_dbg, s_ex_operand_dbg;
    sc_signal<sc_uint<16>> s_wb_opcode_dbg, s_wb_operand_dbg;

    void wb_process() {
        sc_uint<16> wb_data = 0;
        sc_uint<8> op_wb = s_wb_opcode.read().to_uint();
        if ((op_wb & 0xFC) == 0x20) { // LDI
            wb_data = s_wb_operand.read();
        } else if ((op_wb & 0xFC) == 0x10 || (op_wb & 0xFC) == 0x40) { // LD or LD_IND
            wb_data = s_wb_ram_data.read();
        } else if (op_wb == OP_MOV) { // MOV
            wb_data = s_wb_rf_data1.read();
        } else {
            wb_data = s_wb_alu_result.read(); // ADD/SUB
        }
        s_wb_rf_data_in.write(wb_data);
    }

    void id_process() {
        sc_uint<16> id1 = s_rf_data1.read();
        sc_uint<16> id2 = s_rf_data2.read();
        sc_uint<16> wb_data = s_wb_rf_data_in.read();
 
        // WB->ID Forwarding
        if (s_wb_rf_we.read()) {
            if (s_wb_rf_idx_w.read() == s_id_rf_idx_r1.read()) id1 = wb_data;
            if (s_wb_rf_idx_w.read() == s_id_rf_idx_r2.read()) id2 = wb_data;
        }
         
        s_rf_data1_fwd.write(id1);
        s_rf_data2_fwd.write(id2);

        sc_uint<8> op_decoded = 0;
        if (s_id_opcode_raw.read().is_01()) {
            op_decoded = s_id_opcode_raw.read().to_uint();
        }
        s_id_opcode.write(op_decoded);
        
        s_pc_en.write(!s_id_pc_load.read());
        s_if_flush.write(s_id_pc_load.read());
    }

    void ex_process() {
        sc_uint<16> val1 = s_ex_rf_data1.read();
        sc_uint<16> val2 = s_ex_rf_data2.read();
        sc_uint<16> wb_data = s_wb_rf_data_in.read();
 
        // WB->EX Forwarding
        if (s_wb_rf_we.read()) {
            if (s_wb_rf_idx_w.read() == s_ex_rf_idx_r1.read()) val1 = wb_data;
            if (s_wb_rf_idx_w.read() == s_ex_rf_idx_r2.read()) val2 = wb_data;
        }
        s_ex_rf_data1_fwd.write(val1);
        s_ex_rf_data2_fwd.write(val2);

        sc_uint<8> op_ex = s_ex_opcode.read().to_uint();
        if (op_ex == OP_ADD_RR || op_ex == OP_SUB_RR) {
            s_ex_alu_b_in.write(val2);
        } else {
            s_ex_alu_b_in.write(s_mem_data_mux.read()); 
        }

        if (s_ex_is_indirect.read()) {
            s_ex_ram_addr.write(val2);
        } else {
            s_ex_ram_addr.write(s_ex_operand.read());
        }

        // Calculate EX data for EX->ID forwarding 
        // LDI result is imm, ADD/SUB/MOV is alu_result. 
        // Direct LD is safe to forward since its address is constant.
        sc_uint<16> ex_data = 0;
        if ((op_ex & 0xFC) == 0x20) {
            ex_data = s_ex_operand.read();
        }
        else if ((op_ex & 0xFC) == 0x10) ex_data = s_mem_data_mux.read(); // Regular LD
        else if (op_ex == OP_MOV) ex_data = val1;
        else ex_data = s_ex_alu_result.read();
        s_ex_data_fwd.write(ex_data);
    }

    void pipeline_glue() {
        s_true.write(1); s_false.write(0);

        sc_uint<16> inst = s_rom_data.read();
        s_rom_opcode.write(inst.range(15, 8).to_uint());
        s_rom_operand.write(inst.range(7, 0).to_uint());
        
        bool fz = s_flags_z.read();
        bool fc = s_flags_c.read();
        bool fn = s_flags_n.read();
        bool fo = s_flags_o.read();
        if (s_ex_alu_en.read()) {
            fz = s_ex_alu_zero.read();
            fc = s_ex_alu_carry.read();
            fn = s_ex_alu_negative.read();
            fo = s_ex_alu_overflow.read();
        }
        s_id_fwd_z.write(fz);
        s_id_fwd_c.write(fc);
        s_id_fwd_n.write(fn);
        s_id_fwd_o.write(fo);

        // Trace signals
        s_pc_to_bus.write(s_id_pc.read());
        s_ir_opcode.write(s_id_opcode_raw.read());
        s_ir_operand.write(s_id_operand_raw.read());
        s_mar_to_ram.write(s_ex_ram_addr.read());
        s_ram_to_bus.write(s_mem_data_mux.read());
        common_bus.write(s_wb_rf_data_in.read());
        s_wb_out_data.write(s_wb_rf_data1.read());
        s_rega_out.write(s_rf_r0.read());

        s_if_pc_dbg.write(s_pc_out.read());
        s_id_pc_dbg.write(s_id_pc.read());
        s_ex_pc_dbg.write(s_ex_pc_dbg_internal.read());
        s_wb_pc_dbg.write(s_wb_pc_dbg_internal.read());
        sc_uint<16> op_if = inst.range(15, 8).to_uint();
        sc_uint<16> opr_if = inst.range(7, 0).to_uint();
        
        s_if_opcode_dbg.write(op_if);
        s_if_operand_dbg.write(opr_if);
        s_ex_opcode_dbg.write(s_ex_opcode.read());
        s_ex_operand_dbg.write(s_ex_operand.read());
        s_wb_opcode_dbg.write(s_wb_opcode.read());
        s_wb_operand_dbg.write(s_wb_operand.read());
    }

    sc_signal<sc_uint<16>> s_ex_pc_dbg_internal;
    sc_signal<sc_uint<16>> s_wb_pc_dbg_internal;
    sc_signal<sc_uint<16>> s_rega_out; // compatibility
 
    sc_signal<sc_uint<16>> s_ex_data_fwd; // New intermediate signal to break loop

    SC_CTOR(ComputerTop) {
        SC_METHOD(wb_process);
        sensitive << s_wb_opcode << s_wb_operand << s_wb_ram_data << s_wb_alu_result << s_wb_rf_data1;

        SC_METHOD(id_process);
        sensitive << s_rf_data1 << s_rf_data2 << s_wb_rf_we << s_wb_rf_idx_w << s_wb_rf_data_in 
                  << s_ex_rf_we << s_ex_rf_idx_w << s_ex_data_fwd << s_id_rf_idx_r1 << s_id_rf_idx_r2
                  << s_id_opcode_raw << s_id_operand_raw << s_id_pc_load;

        SC_METHOD(ex_process);
        sensitive << s_ex_rf_data1 << s_ex_rf_data2 << s_wb_rf_we << s_wb_rf_idx_w << s_wb_rf_data_in
                  << s_ex_rf_idx_r1 << s_ex_rf_idx_r2 << s_ex_opcode << s_ex_operand << s_ram_out
                  << s_ex_is_indirect << s_ex_alu_result;

        SC_METHOD(pipeline_glue);
        sensitive << clk << s_rom_data << s_id_pc << s_ex_opcode << s_ex_operand 
                  << s_ex_pc_dbg_internal << s_wb_pc_dbg_internal
                  << s_ex_alu_zero << s_ex_alu_carry << s_ex_alu_negative << s_ex_alu_en
                  << s_flags_z << s_flags_c << s_flags_n << s_ex_ram_addr
                  << s_ram_out << s_vram_out << s_mem_data_mux;

        SC_METHOD(mem_decoder);
        sensitive << s_ex_ram_addr << s_ex_ram_we << s_ram_out << s_vram_out;

        Pc = new ProgramCounter("PC");
        Pc->clk(clk); Pc->reset(reset); Pc->en(s_pc_en); Pc->load(s_id_pc_load); Pc->data_in(s_id_operand); Pc->q(s_pc_out);

        InstRom = new ROM("InstRom");
        InstRom->addr(s_pc_out);
        InstRom->data_out(s_rom_data);

        IfId = new IF_ID_Reg("IF_ID");
        IfId->clk(clk); IfId->reset(reset); IfId->enable(s_true); IfId->flush(s_if_flush);
        IfId->pc_in(s_pc_out); IfId->opcode_in(s_rom_opcode); IfId->operand_in(s_rom_operand);
        IfId->pc_out(s_id_pc); IfId->opcode_out(s_id_opcode_raw); IfId->operand_out(s_id_operand);

        Cu = new ControlUnit("CU");
        Cu->opcode(s_id_opcode); Cu->operand(s_id_operand); 
        Cu->flag_z(s_id_fwd_z); Cu->flag_c(s_id_fwd_c); Cu->flag_n(s_id_fwd_n); Cu->flag_o(s_id_fwd_o);
        Cu->rf_we(s_id_rf_we); Cu->rf_idx_w(s_id_rf_idx_w); Cu->rf_idx_r1(s_id_rf_idx_r1); Cu->rf_idx_r2(s_id_rf_idx_r2);
        Cu->rf_out_en(s_id_rf_out_en);
        Cu->alu_op(s_id_alu_op); Cu->alu_en(s_id_alu_en); Cu->ram_we(s_id_ram_we); Cu->is_mem_access(s_id_is_mem_access);
        Cu->is_store(s_id_is_store); Cu->out_load(s_id_out_load); Cu->pc_load(s_id_pc_load);
        Cu->is_indirect(s_id_is_indirect);

        Rf = new RegisterFile("RF");
        Rf->clk(clk); Rf->reset(reset);
        Rf->we(s_wb_rf_we); Rf->idx_w(s_wb_rf_idx_w);
        Rf->idx_r1(s_id_rf_idx_r1); Rf->idx_r2(s_id_rf_idx_r2);
        Rf->data_in(s_wb_rf_data_in);
        Rf->data_out1(s_rf_data1); Rf->data_out2(s_rf_data2);
        Rf->r0_out(s_rf_r0); Rf->r1_out(s_rf_r1); Rf->r2_out(s_rf_r2); Rf->r3_out(s_rf_r3);

        IdEx = new ID_EX_Reg("ID_EX");
        IdEx->clk(clk); IdEx->reset(reset); IdEx->enable(s_true); IdEx->flush(s_false);
        IdEx->alu_op_in(s_id_alu_op); IdEx->b_in_ctrl(s_id_alu_en);
        IdEx->rf_we_in(s_id_rf_we); IdEx->rf_idx_w_in(s_id_rf_idx_w);
        IdEx->rf_idx_r1_in(s_id_rf_idx_r1); IdEx->rf_idx_r2_in(s_id_rf_idx_r2);
        IdEx->out_load_ctrl(s_id_out_load); IdEx->ram_we_ctrl(s_id_ram_we);
        IdEx->operand_in(s_id_operand); IdEx->rf_data1_in(s_rf_data1_fwd); IdEx->rf_data2_in(s_rf_data2_fwd);
        IdEx->opcode_in(s_id_opcode); IdEx->pc_in(s_id_pc);
        IdEx->alu_op_out(s_ex_alu_op); IdEx->b_in_out(s_ex_alu_en);
        IdEx->rf_we_out(s_ex_rf_we); IdEx->rf_idx_w_out(s_ex_rf_idx_w);
        IdEx->rf_idx_r1_out(s_ex_rf_idx_r1); IdEx->rf_idx_r2_out(s_ex_rf_idx_r2);
        IdEx->out_load_out(s_ex_out_load); IdEx->ram_we_out(s_ex_ram_we);
        IdEx->operand_out(s_ex_operand); IdEx->rf_data1_out(s_ex_rf_data1); IdEx->rf_data2_out(s_ex_rf_data2);
        IdEx->opcode_out(s_ex_opcode); IdEx->pc_out(s_ex_pc_dbg_internal);
        IdEx->is_indirect_in(s_id_is_indirect); IdEx->is_indirect_out(s_ex_is_indirect);

        DataRam = new RAM("DataRam");
        DataRam->clk(clk); DataRam->we(s_ram_we_mux); DataRam->addr(s_ex_ram_addr);
        DataRam->data_in(s_ex_rf_data1_fwd); DataRam->data_out(s_ram_out);

        DataVram = new VRAM("DataVram");
        DataVram->clk(clk); DataVram->we(s_vram_we_mux); DataVram->addr(s_ex_ram_addr);
        DataVram->data_in(s_ex_rf_data1_fwd); DataVram->data_out(s_vram_out);

        Alu = new ALU("ALU");
        Alu->a(s_ex_rf_data1_fwd); Alu->b(s_ex_alu_b_in); Alu->op(s_ex_alu_op); Alu->result(s_ex_alu_result);
        Alu->zero_flag(s_ex_alu_zero); Alu->carry_flag(s_ex_alu_carry); Alu->negative_flag(s_ex_alu_negative);
        Alu->overflow_flag(s_ex_alu_overflow);

        ExWb = new EX_WB_Reg("EX_WB");
        ExWb->clk(clk); ExWb->reset(reset); ExWb->enable(s_true);
        ExWb->rf_we_ctrl(s_ex_rf_we); ExWb->rf_idx_w_in(s_ex_rf_idx_w);
        ExWb->out_load_ctrl(s_ex_out_load); ExWb->ram_we_ctrl(s_ex_ram_we);
        ExWb->alu_result_in(s_ex_alu_result); ExWb->operand_in(s_ex_operand);
        ExWb->opcode_in(s_ex_opcode); ExWb->rf_data1_in(s_ex_rf_data1_fwd); ExWb->ram_data_in(s_ram_out);
        ExWb->rf_we_out(s_wb_rf_we); ExWb->rf_idx_w_out(s_wb_rf_idx_w);
        ExWb->out_load_out(s_wb_out_load); ExWb->ram_we_out(s_wb_ram_we);
        ExWb->alu_result_out(s_wb_alu_result); ExWb->operand_out(s_wb_operand);
        ExWb->opcode_out(s_wb_opcode); ExWb->rf_data1_out(s_wb_rf_data1); ExWb->ram_data_out(s_wb_ram_data);
        ExWb->pc_in(s_ex_pc_dbg_internal); ExWb->pc_out(s_wb_pc_dbg_internal);

        OutReg = new OutputRegister("OUT_REG");
        OutReg->clk(clk); OutReg->load(s_wb_out_load); OutReg->data_in(s_wb_out_data); 
        OutReg->display(s_display_val);

        SC_METHOD(update_flags);
        sensitive << clk.pos();
    }

    void update_flags() {
        if (reset.read()) {
            s_flags_z.write(0); s_flags_c.write(0); s_flags_n.write(0); s_flags_o.write(0);
        } else if (s_ex_alu_en.read()) {
             s_flags_z.write(s_ex_alu_zero.read());
             s_flags_c.write(s_ex_alu_carry.read());
             s_flags_n.write(s_ex_alu_negative.read());
             s_flags_o.write(s_ex_alu_overflow.read());
        }
    }

    void mem_decoder() {
        uint16_t addr_val = s_ex_ram_addr.read().to_uint();
        bool we = s_ex_ram_we.read();
        
        // RAM: 0x0000 - 0x00FF (0-255)
        // VRAM: 0x0100 - 0x01FF (256-511)
        if (addr_val >= 256 && addr_val < 512) {
            s_ram_we_mux.write(false);
            s_vram_we_mux.write(we);
            s_mem_data_mux.write(s_vram_out.read());
        } else {
            s_ram_we_mux.write(we);
            s_vram_we_mux.write(false);
            s_mem_data_mux.write(s_ram_out.read());
        }
    }
};

#endif // COMPUTER_TOP_H