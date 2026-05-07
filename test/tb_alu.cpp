#include <systemc.h>
#include <iomanip>
#include "alu.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    sc_signal<sc_uint<16>> sig_a, sig_b, sig_res;
    sc_signal<sc_uint<2>> sig_op;
    sc_signal<bool> sig_zero, sig_carry, sig_neg, sig_ovf;

    ALU Alu("arithmetic_logic_unit");
    Alu.a(sig_a);
    Alu.b(sig_b);
    Alu.op(sig_op);
    Alu.result(sig_res);
    Alu.zero_flag(sig_zero);
    Alu.carry_flag(sig_carry);
    Alu.negative_flag(sig_neg);
    Alu.overflow_flag(sig_ovf);

    std::cout << "KHR-16 ALU Testbench (16-bit Behavioral)\n";
    std::cout << "Time  | Op  | A      | B      || Res    | Z C N V | Description\n";
    std::cout << "--------------------------------------------------------------\n";

    auto print_line = [&](std::string desc) {
        std::cout << std::setw(5) << sc_time_stamp() << " | "
                  << std::setw(3) << sig_op.read().to_uint() << " | "
                  << std::setw(6) << (int16_t)sig_a.read().to_uint() << " | "
                  << std::setw(6) << (int16_t)sig_b.read().to_uint() << " || "
                  << std::setw(6) << (int16_t)sig_res.read().to_uint() << " | "
                  << sig_zero.read() << " " << sig_carry.read() << " " 
                  << sig_neg.read() << " " << sig_ovf.read() << " | " << desc << "\n";
    };

    // 1. ADD (10 + 20 = 30)
    sig_op.write(0); sig_a.write(10); sig_b.write(20);
    sc_start(10, SC_NS);
    print_line("ADD: 10 + 20 = 30");

    // 2. SUB (50 - 50 = 0) -> Zero flag
    sig_op.write(1); sig_a.write(50); sig_b.write(50);
    sc_start(10, SC_NS);
    print_line("SUB: 50 - 50 = 0 (Z=1)");

    // 3. SUB (10 - 20 = -10) -> Negative flag
    sig_op.write(1); sig_a.write(10); sig_b.write(20);
    sc_start(10, SC_NS);
    print_line("SUB: 10 - 20 = -10 (N=1)");

    // 4. ADD Overflow (32767 + 1) -> Overflow flag (V=1)
    sig_op.write(0); sig_a.write(32767); sig_b.write(1);
    sc_start(10, SC_NS);
    print_line("ADD: 32767 + 1 = -32768 (V=1)");

    // 5. AND (0x0F0F & 0xF0F0 = 0)
    sig_op.write(2); sig_a.write(0x0F0F); sig_b.write(0xF0F0);
    sc_start(10, SC_NS);
    print_line("AND: 0x0F0F & 0xF0F0 = 0");

    return 0;
}