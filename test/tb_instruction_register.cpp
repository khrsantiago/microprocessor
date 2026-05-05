#include <systemc.h>
#include "instruction_register.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    sc_clock clk("sys_clk", 10, SC_NS);
    sc_signal<bool> sig_ld_opcode, sig_ld_operand;
    sc_signal<sc_lv<8>> sig_data_in;
    sc_signal<sc_lv<8>> sig_opcode, sig_operand;

    InstructionRegister Dut("Main_IR");
    Dut.clk(clk);
    Dut.ld_opcode(sig_ld_opcode);
    Dut.ld_operand(sig_ld_operand);
    Dut.data_in(sig_data_in);
    Dut.opcode_out(sig_opcode);
    Dut.operand_out(sig_operand);

    std::cout << "Sequential Unit: Instruction Register (IR)\n";
    std::cout << "Time  | LD |  IN_Bus  || Out_Opcode | Out_Operand | Action\n";
    std::cout << "-------------------------------------------------------------------------\n";

    // 1. Cargar Instruccion (Dual Step): LDA 5
    // Paso A: Cargar Opcode (LDA = 0x01)
    sig_ld_opcode.write(1); sig_ld_operand.write(0);
    sig_data_in.write("00000001"); 
    sc_start(10, SC_NS); 

    // Paso B: Cargar Operando (5 = 0x05)
    sig_ld_opcode.write(0); sig_ld_operand.write(1);
    sig_data_in.write("00000101");
    sc_start(10, SC_NS);
    
    std::cout << sc_time_stamp() << " |  D  |     " 
              << sig_data_in.read() << "   ||     " << sig_opcode.read() 
              << "     |     " << sig_operand.read() << "      | LDA 5 (Dual-Fetch)\n";

    // 2. Apagar carga y cambiar bus (Simulando que el bus se usa para otra cosa)
    sig_ld_opcode.write(0); sig_ld_operand.write(0);
    sig_data_in.write("11111111"); // Ruido en el bus
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " |  0  |     " 
              << sig_data_in.read() << "   ||     " << sig_opcode.read() 
              << "     |     " << sig_operand.read() << "      | Bus busy, IR holds LDA 5\n";

    // 3. Cargar Nueva Instruccion: OUT (0x02 0x00)
    sig_ld_opcode.write(1); sig_ld_operand.write(0);
    sig_data_in.write("00000010");
    sc_start(10, SC_NS);
    sig_ld_opcode.write(0); sig_ld_operand.write(1);
    sig_data_in.write("00000000");
    sc_start(10, SC_NS);

    std::cout << sc_time_stamp() << " |  D  |     " 
              << sig_data_in.read() << "   ||     " << sig_opcode.read() 
              << "     |     " << sig_operand.read() << "      | OUT\n";

    return 0;
}