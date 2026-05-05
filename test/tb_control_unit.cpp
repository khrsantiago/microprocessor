#include <systemc.h>
#include "control_unit.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    sc_clock clk("sys_clk", 10, SC_NS);
    sc_signal<bool> sig_reset;
    sc_signal<sc_uint<8>> sig_opcode;
    
    // Cables de control
    sc_signal<bool> s_pc_out, s_mar_in, s_pc_inc, s_ram_out, s_ir_ld_opcode, s_ir_ld_operand, 
                    s_ir_out, s_a_in, s_pc_load, s_flag_z, s_out_load, 
                    s_a_out, s_b_in, s_alu_out, s_ram_we;
    sc_signal<sc_uint<2>> s_alu_op;

    ControlUnit Cu("main_control_unit");
    Cu.clk(clk); Cu.reset(sig_reset);
    Cu.pc_out(s_pc_out); Cu.mar_in(s_mar_in); Cu.pc_inc(s_pc_inc);
    Cu.ram_out(s_ram_out); Cu.ir_ld_opcode(s_ir_ld_opcode); Cu.ir_ld_operand(s_ir_ld_operand);
    Cu.opcode(sig_opcode);
    Cu.ir_out(s_ir_out); Cu.a_in(s_a_in);
    Cu.pc_load(s_pc_load); Cu.flag_z(s_flag_z);
    Cu.out_load(s_out_load); Cu.a_out(s_a_out);
    Cu.b_in(s_b_in); Cu.alu_out(s_alu_out);
    Cu.alu_op(s_alu_op); Cu.ram_we(s_ram_we);

    std::cout << "Control Unit FSM - Fetch Cycle Timing\n";
    std::cout << "Time  | State | PC_out | MAR_in | PC_inc | RAM_out | IR_in | Action\n";
    std::cout << "-----------------------------------------------------------------------\n";

    // Inicializacion
    sig_reset.write(1);
    sc_start(5, SC_NS);
    sig_reset.write(0);

    // Simulamos 5 ciclos de reloj (Un Dual-Fetch completo: T1-T5)
    for(int i = 0; i < 5; i++) {
        sc_start(10, SC_NS);
        std::cout << sc_time_stamp() << " |  T" << (i+1) << "   |   " 
                  << s_pc_out.read() << "    |   " << s_mar_in.read() << "    |   " 
                  << s_pc_inc.read() << "    |    " << s_ram_out.read() << "    |   " 
                  << s_ir_ld_opcode.read() << "/" << s_ir_ld_operand.read() << " | ";
                  
        if(i == 0) std::cout << "Address -> MAR\n";
        if(i == 1) std::cout << "Increment PC\n";
        if(i == 2) std::cout << "RAM Data -> IR\n";
    }

    return 0;
}