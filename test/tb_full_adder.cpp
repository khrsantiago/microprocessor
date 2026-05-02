#include <systemc.h>
#include "full_adder.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    // 1. Cables
    sc_signal<bool> sig_a, sig_b, sig_cin;
    sc_signal<bool> sig_sum, sig_cout;

    // 2. Instanciar el DUT (Device Under Test)
    FullAdder Dut("my_full_adder");
    Dut.a(sig_a);
    Dut.b(sig_b);
    Dut.cin(sig_cin);
    Dut.sum(sig_sum);
    Dut.cout(sig_cout);

    // 3. Pruebas (Las 8 combinaciones de la tabla de verdad)
    std::cout << "Starting Full Adder Base Test...\n";
    std::cout << "Time   | A | B | Cin || Sum | Cout\n";
    std::cout << "-------------------------------------\n";

    // Prueba 1: 0 + 0 + 0
    sig_a.write(0); sig_b.write(0); sig_cin.write(0);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << "  | " 
            << sig_a.read() << " | " 
            << sig_b.read() << " |  " 
            << sig_cin.read() << "  ||  " 
            << sig_sum.read() << "   |  " 
            << sig_cout.read() << "\n";

    // Prueba 2: 0 + 0 + 1
    sig_a.write(0); sig_b.write(0); sig_cin.write(1);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << "  | " 
            << sig_a.read() << " | " 
            << sig_b.read() << " |  " 
            << sig_cin.read() << "  ||  " 
            << sig_sum.read() << "   |  " 
            << sig_cout.read() << "\n";

    // Prueba 3: 0 + 1 + 0
    sig_a.write(0); sig_b.write(1); sig_cin.write(0);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << "  | " 
            << sig_a.read() << " | " 
            << sig_b.read() << " |  " 
            << sig_cin.read() << "  ||  " 
            << sig_sum.read() << "   |  " 
            << sig_cout.read() << "\n";

    // Prueba 4: 0 + 1 + 1  (Suma debe dar 0, Acarreo 1)
    sig_a.write(0); sig_b.write(1); sig_cin.write(1);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << "  | " 
            << sig_a.read() << " | " 
            << sig_b.read() << " |  " 
            << sig_cin.read() << "  ||  " 
            << sig_sum.read() << "   |  " 
            << sig_cout.read() << "\n";

    // Prueba 5: 1 + 0 + 0
    sig_a.write(1); sig_b.write(0); sig_cin.write(0);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << "  | " 
            << sig_a.read() << " | " 
            << sig_b.read() << " |  " 
            << sig_cin.read() << "  ||  " 
            << sig_sum.read() << "   |  " 
            << sig_cout.read() << "\n";

    // Prueba 6: 1 + 0 + 1  (Suma debe dar 0, Acarreo 1)
    sig_a.write(1); sig_b.write(0); sig_cin.write(1);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << "  | " 
            << sig_a.read() << " | " 
            << sig_b.read() << " |  " 
            << sig_cin.read() << "  ||  " 
            << sig_sum.read() << "   |  " 
            << sig_cout.read() << "\n";

    // Prueba 7: 1 + 1 + 0  (Suma debe dar 0, Acarreo 1)
    sig_a.write(1); sig_b.write(1); sig_cin.write(0);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << "  | " 
            << sig_a.read() << " | " 
            << sig_b.read() << " |  " 
            << sig_cin.read() << "  ||  " 
            << sig_sum.read() << "   |  " 
            << sig_cout.read() << "\n";

    // Prueba 8: 1 + 1 + 1  (Suma debe dar 1, Acarreo 1)
    sig_a.write(1); sig_b.write(1); sig_cin.write(1);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << "  | " 
            << sig_a.read() << " | " 
            << sig_b.read() << " |  " 
            << sig_cin.read() << "  ||  " 
            << sig_sum.read() << "   |  " 
            << sig_cout.read() << "\n";

    return 0;
}