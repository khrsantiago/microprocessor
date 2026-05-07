#include <systemc.h>
#include <iostream>
#include <iomanip>
#include "output_register.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    sc_clock clk("clk", 10, SC_NS);
    sc_signal<bool> load;
    sc_signal<sc_lv<16>> data_in;
    sc_signal<sc_uint<16>> display;

    OutputRegister OutReg("Output_Register");
    OutReg.clk(clk);
    OutReg.load(load);
    OutReg.data_in(data_in);
    OutReg.display(display);

    std::cout << "Starting Output Register Unit Test (16-bit)...\n";
    std::cout << "Time  | Load | Data_In          | Display (Expected)\n";
    std::cout << "----------------------------------------------------\n";

    load.write(0);
    data_in.write("0000000000000000");
    sc_start(5, SC_NS);

    // --- PRUEBA 1: Aislamiento ---
    data_in.write("0000000000000101"); // 5
    sc_start(10, SC_NS);
    std::cout << std::setw(5) << sc_time_stamp() << " |  " 
              << load.read() << "   |  " << data_in.read() << " | " 
              << display.read() << " (Should be 0)\n";

    // --- PRUEBA 2: Captura Exitosa ---
    load.write(1);
    sc_start(10, SC_NS);
    std::cout << std::setw(5) << sc_time_stamp() << " |  " 
              << load.read() << "   |  " << data_in.read() << " | " 
              << display.read() << " (Should be 5)\n";

    // --- PRUEBA 3: Resistencia a Alta Impedancia (Z) ---
    data_in.write("ZZZZZZZZZZZZZZZZ");
    sc_start(10, SC_NS);
    std::cout << std::setw(5) << sc_time_stamp() << " |  " 
              << load.read() << "   |  " << data_in.read() << " | " 
              << display.read() << " (Should remain 5)\n";

    // --- PRUEBA 4: Gran Captura (16 bits) ---
    data_in.write("1111111111111111"); // 65535 (FFFF)
    sc_start(10, SC_NS);
    std::cout << std::setw(5) << sc_time_stamp() << " |  " 
              << load.read() << "   |  " << data_in.read() << " | " 
              << display.read() << " (Should be 65535)\n";

    std::cout << "\nTest Complete.\n";
    return 0;
}