#include <systemc.h>
#include "adder_subtractor.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    sc_signal<sc_uint<4>> sig_a, sig_b, sig_res;
    sc_signal<bool> sig_op, sig_cout, sig_z, sig_n, sig_v;

    AdderSubtractor Alu("alu_complete");
    Alu.a(sig_a); Alu.b(sig_b); Alu.op(sig_op);
    Alu.result(sig_res); Alu.cout(sig_cout);
    Alu.zero(sig_z); Alu.negative(sig_n); Alu.overflow(sig_v);

    std::cout << "Overflow Flag Test (V)\n";
    std::cout << "Time | A | B | Op || Res | V | Comment\n";
    std::cout << "----------------------------------------------\n";

    // Case 1: 7 + 1 (Positive + Positive = Negative?)
    sig_op.write(0); sig_a.write(7); sig_b.write(1);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " | 7 | 1 |  + ||  " << sig_res.read() 
              << "  | " << sig_v.read() << " | 7+1=8 (Exceeds +7)\n";

    // Case 2: -4 - 5 (Negative + Negative = Positive?)
    // In 4 bits: -4 is 1100 (12 decimal) and -5 is 1011 (11 decimal)
    sig_op.write(0); sig_a.write(12); sig_b.write(11);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " |12 |11 |  + ||  " << sig_res.read() 
              << "  | " << sig_v.read() << " | -4-5=-9 (Exceeds -8)\n";

    // Case 3: 5 + 2 (Normal addition within range)
    sig_op.write(0); sig_a.write(5); sig_b.write(2);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " | 5 | 2 |  + ||  " << sig_res.read() 
              << "  | " << sig_v.read() << " | No overflow\n";

    return 0;
}