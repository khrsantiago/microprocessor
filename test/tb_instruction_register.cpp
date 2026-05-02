#include <systemc.h>
#include "instruction_register.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    sc_clock clk("sys_clk", 10, SC_NS);
    sc_signal<bool> sig_ld;
    sc_signal<sc_lv<8>> sig_data_in;
    sc_signal<sc_lv<4>> sig_opcode, sig_operand;

    InstructionRegister Dut("Main_IR");
    Dut.clk(clk);
    Dut.ld(sig_ld);
    Dut.data_in(sig_data_in);
    Dut.opcode_out(sig_opcode);
    Dut.operand_out(sig_operand);

    std::cout << "Sequential Unit: Instruction Register (IR)\n";
    std::cout << "Time  | LD |  IN_Bus  || Out_Opcode | Out_Operand | Action\n";
    std::cout << "-------------------------------------------------------------------------\n";

    // 1. Cargar Instruccion: ADD 5 (0010 0101)
    sig_ld.write(1);
    sig_data_in.write("00100101");
    sc_start(10, SC_NS); 
    std::cout << sc_time_stamp() << " |  " << sig_ld.read() << " |     " 
              << sig_data_in.read() << "   ||     " << sig_opcode.read() 
              << "     |     " << sig_operand.read() << "      | ADD 5\n";

    // 2. Apagar carga y cambiar bus (Simulando que el bus se usa para otra cosa)
    sig_ld.write(0);
    sig_data_in.write("11111111"); // Ruido en el bus
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " |  " << sig_ld.read() << " |     " 
              << sig_data_in.read() << "   ||     " << sig_opcode.read() 
              << "     |     " << sig_operand.read() << "      | Bus busy, IR holds ADD 5\n";

    // 3. Cargar Nueva Instruccion: LDA 12 (0001 1100)
    sig_ld.write(1);
    sig_data_in.write("00011100");
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " |  " << sig_ld.read() << " |     " 
              << sig_data_in.read() << "   ||     " << sig_opcode.read() 
              << "     |     " << sig_operand.read() << "      | LDA 12\n";

    return 0;
}