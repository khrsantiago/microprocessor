#include <systemc.h>
#include "alu.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    sc_signal<sc_uint<8>> sig_a, sig_b, sig_res;
    sc_signal<sc_uint<2>> sig_op;
    sc_signal<bool> sig_zero;

    ALU Alu("arithmetic_logic_unit");
    Alu.a(sig_a);
    Alu.b(sig_b);
    Alu.op(sig_op);
    Alu.result(sig_res);
    Alu.zero_flag(sig_zero);

    std::cout << "ALU Testbench (MUX 4-to-1 & Zero Flag)\n";
    std::cout << "Time  | Op (MUX) | A | B || Res | Z_Flag | Description\n";
    std::cout << "--------------------------------------------------------\n";

    // Prueba 00: ADD (2 + 3)
    sig_op.write(0); sig_a.write(2); sig_b.write(3);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " |    00    | 2 | 3 ||  " 
              << sig_res.read() << "  |   " << sig_zero.read() << "    | ADD: 2+3=5\n";

    // Prueba 01: SUB (8 - 8) -> Deberia activar Zero Flag
    sig_op.write(1); sig_a.write(8); sig_b.write(8);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " |    01    | 8 | 8 ||  " 
              << sig_res.read() << "  |   " << sig_zero.read() << "    | SUB: 8-8=0 (Z=1)\n";

    // Prueba 10: AND bit a bit (12 y 10)
    sig_op.write(2); sig_a.write(12); sig_b.write(10);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " |    10    |12 |10 ||  " 
              << sig_res.read() << "  |   " << sig_zero.read() << "    | AND: 1100 & 1010 = 1000 (8) [Operando de 8 bits]\n";

    // Prueba 11: OR bit a bit (4 y 1)
    sig_op.write(3); sig_a.write(4); sig_b.write(1);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " |    11    | 4 | 1 ||  " 
              << sig_res.read() << "  |   " << sig_zero.read() << "    | OR : 0100 | 0001 = 0101 (5)\n";

    return 0;
}