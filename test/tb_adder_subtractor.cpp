#include <systemc.h>
#include "adder_subtractor.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    // 1. Definición de cables
    sc_signal<sc_uint<4>> sig_a, sig_b, sig_res;
    sc_signal<bool> sig_op, sig_cout, sig_z, sig_n, sig_o;

    // 2. Instancia del componente
    AdderSubtractor Dut("arithmetic_alu");
    Dut.a(sig_a);
    Dut.b(sig_b);
    Dut.op(sig_op);
    Dut.result(sig_res);
    Dut.cout(sig_cout);
    Dut.zero(sig_z);
    Dut.negative(sig_n);
    Dut.overflow(sig_o);

    std::cout << "Starting Arithmetic Unit Test (Sum/Sub)...\n";
    std::cout << "Time  | Op | A | B || Res | Cout | Real Operation\n";
    std::cout << "------------------------------------------------------\n";

    // CASO 1: Suma simple (5 + 3)
    sig_op.write(0); sig_a.write(5); sig_b.write(3);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " | " << sig_op.read() << "  | " << sig_a.read() << " | " << sig_b.read() 
              << " ||  " << sig_res.read() << "  |  " << sig_cout.read() << "   | 5 + 3 = 8\n";

    // CASO 2: Resta con resultado positivo (5 - 3)
    // 5 - 3 = 5 + (Invertir 3) + 1
    sig_op.write(1); sig_a.write(5); sig_b.write(3);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " | " << sig_op.read() << "  | " << sig_a.read() << " | " << sig_b.read() 
              << " ||  " << sig_res.read() << "  |  " << sig_cout.read() << "   | 5 - 3 = 2\n";

    // CASO 3: Resta con resultado negativo (3 - 5)
    // El resultado sera 14 (que es -2 en complemento a 2 de 4 bits)
    sig_op.write(1); sig_a.write(3); sig_b.write(5);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " | " << sig_op.read() << "  | " << sig_a.read() << " | " << sig_b.read() 
              << " || " << sig_res.read() << "  |  " << sig_cout.read() << "   | 3 - 5 = -2 (14 bin)\n";

    // CASO 4: Resta al limite (0 - 1)
    sig_op.write(1); sig_a.write(0); sig_b.write(1);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " | " << sig_op.read() << "  | " << sig_a.read() << " | " << sig_b.read() 
              << " || " << sig_res.read() << "  |  " << sig_cout.read() << "   | 0 - 1 = -1 (15 bin)\n";

    return 0;
}