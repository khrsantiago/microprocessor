#include <systemc.h>
#include "program_counter.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    sc_clock clk("sys_clk", 10, SC_NS);
    sc_signal<bool> sig_reset, sig_en, sig_load;
    sc_signal<sc_uint<4>> sig_data_in, sig_q;

    ProgramCounter Dut("PC_Jump_Unit");
    Dut.clk(clk); Dut.reset(sig_reset); Dut.en(sig_en);
    Dut.load(sig_load); Dut.data_in(sig_data_in); Dut.q(sig_q);

    std::cout << "Testing PC with JUMP Capability\n";
    std::cout << "Time  | EN | LD | Data_In | PC Out | Action\n";
    std::cout << "------------------------------------------------\n";

    // 1. Iniciar conteo normal
    sig_reset.write(0); sig_en.write(1); sig_load.write(0);
    sc_start(30, SC_NS); // PC llega a 3
    std::cout << sc_time_stamp() << " |  " << sig_en.read() << " |  " << sig_load.read() 
              << " |    X    |   " << sig_q.read() << "    | Counting normal...\n";

    // 2. Ejecutar JUMP a la direccion 0 (Bucle)
    sig_load.write(1); sig_data_in.write(0);
    sc_start(10, SC_NS); // Ocurre el flanco de reloj
    std::cout << sc_time_stamp() << " |  " << sig_en.read() << " |  " << sig_load.read() 
              << " |    0    |   " << sig_q.read() << "    | JUMP Triggered! Back to 0\n";

    // 3. Volver al conteo normal
    sig_load.write(0);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " |  " << sig_en.read() << " |  " << sig_load.read() 
              << " |    X    |   " << sig_q.read() << "    | Counting again...\n";

    return 0;
}