#include <systemc.h>
#include "control_unit.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    sc_clock clk("sys_clk", 10, SC_NS);
    sc_signal<bool> sig_reset;
    sc_signal<sc_uint<4>> sig_opcode;
    
    // Cables de salida de la FSM
    sc_signal<bool> s_pc_out, s_mar_in, s_pc_inc, s_pc_load, s_ram_out, s_ir_in, s_ir_out, s_a_in, s_out_load, s_a_out, s_b_in, s_alu_out, s_flag_z;
    sc_signal<sc_uint<2>> s_alu_op;

    ControlUnit Cu("main_cu");
    Cu.clk(clk); Cu.reset(sig_reset); Cu.opcode(sig_opcode);
    Cu.pc_out(s_pc_out); Cu.mar_in(s_mar_in); Cu.pc_inc(s_pc_inc); Cu.pc_load(s_pc_load);
    Cu.ram_out(s_ram_out); Cu.ir_in(s_ir_in); Cu.ir_out(s_ir_out); Cu.a_in(s_a_in);
    Cu.out_load(s_out_load); Cu.a_out(s_a_out); Cu.b_in(s_b_in); Cu.alu_out(s_alu_out);
    Cu.alu_op(s_alu_op); Cu.flag_z(s_flag_z);

    std::cout << "Control Unit FSM - Full Cycle (LDA Instruction)\n";
    std::cout << "Time  | T | OpCode | IR_out | MAR_in || RAM_out | A_in | Action\n";
    std::cout << "----------------------------------------------------------------------\n";

    // 1. Resetear la maquina e inyectar la instruccion LDA
    sig_reset.write(1); sig_opcode.write(1); // 0001 = LDA
    sc_start(5, SC_NS);
    sig_reset.write(0);

    // 2. Correr los 6 estados de tiempo
    for(int i = 0; i < 6; i++) {
        sc_start(10, SC_NS);
        std::cout << sc_time_stamp() << " | " << (i+1) << " |  0001  |   " 
                  << s_ir_out.read() << "    |   " << s_mar_in.read() << "    ||    " 
                  << s_ram_out.read() << "    |  " << s_a_in.read() << "   | ";
                  
        if(i == 0) std::cout << "(Fetch) PC -> MAR\n";
        if(i == 1) std::cout << "(Fetch) PC + 1\n";
        if(i == 2) std::cout << "(Fetch) RAM -> IR\n";
        if(i == 3) std::cout << "(Exec ) IR Operand -> MAR\n";
        if(i == 4) std::cout << "(Exec ) RAM Data -> Accumulator\n";
        if(i == 5) std::cout << "(Exec ) NOP (Cycle End)\n";
    }

    return 0;
}