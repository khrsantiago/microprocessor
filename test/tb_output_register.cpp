#include <systemc.h>
#include <iostream>
#include <iomanip>
#include "output_register.h"

int sc_main(int argc, char* argv[]) {
    // Desactivamos los warnings de SystemC
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);


    // 1. Declaracion de Senales (Los cables de prueba)
    sc_clock clk("clk", 10, SC_NS);
    sc_signal<bool> load;
    sc_signal<sc_lv<4>> data_in;
    sc_signal<sc_uint<4>> display;

    // 2. Instanciacion del Modulo
    OutputRegister OutReg("Output_Register");
    OutReg.clk(clk);
    OutReg.load(load);
    OutReg.data_in(data_in);
    OutReg.display(display);

    std::cout << "Starting Output Register Unit Test...\n";
    std::cout << "Time  | Load | Data_In | Display (Expected)\n";
    std::cout << "-------------------------------------------\n";

    // Inicializacion
    load.write(0);
    data_in.write("0000");
    sc_start(5, SC_NS); // Medio ciclo para alinear el reloj

    // --- PRUEBA 1: Aislamiento (El Bus tiene datos, pero Load esta apagado) ---
    data_in.write("0101"); // Ponemos un 5 en el bus
    sc_start(10, SC_NS);
    std::cout << std::setw(5) << sc_time_stamp() << " |  " 
              << load.read() << "   |  " << data_in.read() << "   | " 
              << display.read() << " (Should be 0)\n";

    // --- PRUEBA 2: Captura Exitosa (Encendemos Load) ---
    load.write(1);
    sc_start(10, SC_NS);
    std::cout << std::setw(5) << sc_time_stamp() << " |  " 
              << load.read() << "   |  " << data_in.read() << "   | " 
              << display.read() << " (Should be 5)\n";

    // --- PRUEBA 3: Resistencia a Alta Impedancia (Z) ---
    // El bus se desconecta (ZZZZ) pero Load sigue activo por error. 
    // El registro NO debe actualizarse ni colapsar.
    data_in.write("ZZZZ");
    sc_start(10, SC_NS);
    std::cout << std::setw(5) << sc_time_stamp() << " |  " 
              << load.read() << "   |  " << data_in.read() << "   | " 
              << display.read() << " (Should remain 5)\n";

    // --- PRUEBA 4: Nueva Captura ---
    data_in.write("1111"); // Ponemos un 15 (F)
    sc_start(10, SC_NS);
    std::cout << std::setw(5) << sc_time_stamp() << " |  " 
              << load.read() << "   |  " << data_in.read() << "   | " 
              << display.read() << " (Should be 15)\n";

    std::cout << "\nTest Complete.\n";
    return 0;
}