#include <systemc.h>
#include "adder_subtractor.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    sc_signal<sc_uint<4>> sig_a, sig_b, sig_res;
    sc_signal<bool> sig_op, sig_cout, sig_z, sig_n, sig_o;

    AdderSubtractor Alu("alu_with_flags");
    Alu.a(sig_a); Alu.b(sig_b); Alu.op(sig_op);
    Alu.result(sig_res); Alu.cout(sig_cout);
    Alu.zero(sig_z); Alu.negative(sig_n); Alu.overflow(sig_o);

    std::cout << "Flags Test (4-bit ALU)\n";
    std::cout << "Time | A | B | Op || Res | Z | N | Comment\n";
    std::cout << "--------------------------------------------------\n";

    // Case 1: Zero Result (4 - 4)
    sig_op.write(1); sig_a.write(4); sig_b.write(4);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " | 4 | 4 |  - ||  " << sig_res.read() 
              << "  | " << sig_z.read() << " | " << sig_n.read() << " | Zero Result (Z=1)\n";

    // Case 2: Negative Result (2 - 5)
    sig_op.write(1); sig_a.write(2); sig_b.write(5);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " | 2 | 5 |  - || " << sig_res.read() 
              << "  | " << sig_z.read() << " | " << sig_n.read() << " | Negative Result (N=1)\n";

    // Case 3: Positive non-zero result (7 + 1)
    sig_op.write(0); sig_a.write(7); sig_b.write(1);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " | 7 | 1 |  + ||  " << sig_res.read() 
              << "  | " << sig_z.read() << " | " << sig_n.read() << " | Positive (Z=0, N=0)\n";

    return 0;
}