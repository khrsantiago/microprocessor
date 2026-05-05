#include <systemc.h>
#include "adder.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    // 1. Cables (Buses de 4 bits y señales de acarreo)
    sc_signal<sc_uint<8>> sig_a;
    sc_signal<sc_uint<8>> sig_b;
    sc_signal<sc_uint<2>> sig_op;
    sc_signal<sc_uint<8>> sig_sum;
    sc_signal<bool> sig_cin, sig_cout;

    // 2. Instanciar el Sumador de 8 Bits (Nuestro DUT)
    Adder8Bits Dut("my_adder_8_bits");
    Dut.a(sig_a);
    Dut.b(sig_b);
    Dut.cin(sig_cin);
    Dut.sum(sig_sum);
    Dut.cout(sig_cout);

    std::cout << "Starting 4-Bit Adder Test (Ripple Carry)...\n";
    std::cout << "Time  |  A  |  B  | Cin || Sum | Cout | Decimal\n";
    std::cout << "------------------------------------------------------\n";

    // Prueba 1: Suma simple sin acarreo (5 + 3)
    sig_a.write(5); sig_b.write(3); sig_cin.write(0);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << "  | " 
              << sig_a.read() << " | " 
              << sig_b.read() << " |  " 
              << sig_cin.read() << "  || " 
              << sig_sum.read() << "  |  " << sig_cout.read() << "   | " 
              << sig_a.read().to_uint() << " + " << sig_b.read().to_uint() << " = " << sig_sum.read().to_uint() << "\n";

    // Prueba 2: Suma con acarreo interno (7 + 1) -> 0111 + 0001
    sig_a.write(7); sig_b.write(1); sig_cin.write(0);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << "  | " 
              << sig_a.read() << " | " 
              << sig_b.read() << " |  " 
              << sig_cin.read() << "  || " 
              << sig_sum.read() << "  |  " << sig_cout.read() << "   | " 
              << sig_a.read().to_uint() << " + " << sig_b.read().to_uint() << " = " << sig_sum.read().to_uint() << "\n";

    // Prueba 3: Generar Acarreo de Salida (Cout) (15 + 1)
    sig_a.write(15); sig_b.write(1); sig_cin.write(0);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << "  | " 
              << sig_a.read() << " | " 
              << sig_b.read() << " |  " 
              << sig_cin.read() << "  || " 
              << sig_sum.read() << "  |  " << sig_cout.read() << "   | " 
              << sig_a.read().to_uint() << " + " << sig_b.read().to_uint() << " = 16 (Cout=1)\n";

    // Prueba 4: Todo al máximo (15 + 15 + 1)
    sig_a.write(15); sig_b.write(15); sig_cin.write(1);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << "  | " 
              << sig_a.read() << " | " 
              << sig_b.read() << " |  " 
              << sig_cin.read() << "  || " 
              << sig_sum.read() << "  |  " << sig_cout.read() << "   | " 
              << sig_a.read().to_uint() << " + " << sig_b.read().to_uint() << " + 1 = 31 (Cout=1)\n";

    return 0;
}