#include <systemc.h>
#include "program_counter.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    sc_clock clk("sys_clk", 10, SC_NS);
    sc_signal<bool> sig_reset, sig_en, sig_load;
    sc_signal<sc_uint<4>> sig_q, sig_data_in;

    ProgramCounter Dut("PC_Unit");
    Dut.clk(clk);
    Dut.reset(sig_reset);
    Dut.en(sig_en);
    Dut.q(sig_q);
    Dut.load(sig_load);
    Dut.data_in(sig_data_in);


    std::cout << "Testing Program Counter (PC) - 4-bit Autonomy\n";
    std::cout << "Time  | RST | EN | PC Out | Action\n";
    std::cout << "-------------------------------------------\n";

    // 1. Inicializacion y Reset
    sig_reset.write(1); sig_en.write(0);
    sc_start(5, SC_NS);
    std::cout << sc_time_stamp() << "  |  " << sig_reset.read() << "  | " << sig_en.read() 
              << "  |   " << sig_q.read() << "    | System Reset\n";

    // 2. Comenzar a caminar
    sig_reset.write(0); sig_en.write(1);
    for(int i=0; i<3; i++) {
        sc_start(10, SC_NS);
        std::cout << sc_time_stamp() << " |  " << sig_reset.read() << "  | " << sig_en.read() 
                  << "  |   " << sig_q.read() << "    | Incrementing...\n";
    }

    // 3. Pausar el PC (Simulando una instruccion larga de varios ciclos)
    sig_en.write(0);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " |  " << sig_reset.read() << "  | " << sig_en.read() 
              << "  |   " << sig_q.read() << "    | Paused (Enable=0)\n";

    // 4. Reset asincrono en medio de un ciclo
    sc_start(2, SC_NS);
    sig_reset.write(1);
    sc_start(1, SC_NS);
    std::cout << sc_time_stamp() << " |  " << sig_reset.read() << "  | " << sig_en.read() 
              << "  |   " << sig_q.read() << "    | Async Reset Triggered!\n";

    return 0;
}