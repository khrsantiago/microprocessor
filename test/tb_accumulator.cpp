#include <systemc.h>
#include "accumulator.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    sc_clock clk("sys_clk", 10, SC_NS);
    sc_signal<bool> sig_ld;
    sc_signal<sc_uint<4>> sig_d, sig_q;

    Accumulator Acc("register_a");
    Acc.clk(clk); Acc.ld(sig_ld); Acc.d(sig_d); Acc.q(sig_q);

    std::cout << "Testing Accumulator with Load Enable (LD)\n";
    std::cout << "Time  | LD | Data In | Accumulator (Q) | Note\n";
    std::cout << "--------------------------------------------------\n";

    // 1. Intentar cargar con LD=0
    sig_ld.write(0); sig_d.write(5);
    sc_start(15, SC_NS); // Pasa un flanco de subida a los 10ns
    std::cout << sc_time_stamp() << " | " << sig_ld.read() << "  |    " << sig_d.read() 
              << "    |        " << sig_q.read() << "        | LD=0: Ignora el 5\n";

    // 2. Cargar con LD=1
    sig_ld.write(1);
    sc_start(10, SC_NS); // Pasa el flanco de subida a los 20ns
    std::cout << sc_time_stamp() << " | " << sig_ld.read() << "  |    " << sig_d.read() 
              << "    |        " << sig_q.read() << "        | LD=1: Guarda el 5\n";

    // 3. Cambiar entrada pero apagar LD
    sig_d.write(10); sig_ld.write(0);
    sc_start(10, SC_NS); // Pasa el flanco de subida a los 30ns
    std::cout << sc_time_stamp() << " | " << sig_ld.read() << "  |    " << sig_d.read() 
              << "   |        " << sig_q.read() << "        | LD=0: Mantiene el 5\n";

    return 0;
}