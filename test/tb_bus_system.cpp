#include <systemc.h>
#include "tri_state_buffer.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    // Definicion de cables
    sc_signal<sc_uint<8>> data_alu, data_ram;
    sc_signal<bool> en_alu, en_ram;
    // sc_signal_rv<8> common_bus; // El Bus compartido
    sc_signal_rv<8> common_bus;

    // Instanciamos dos interfaces de bus
    TriStateBuffer BufferAlu("buffer_alu");
    BufferAlu.data_in(data_alu);
    BufferAlu.enable(en_alu);
    BufferAlu.bus_out(common_bus);

    TriStateBuffer BufferRam("buffer_ram");
    BufferRam.data_in(data_ram);
    BufferRam.enable(en_ram);
    BufferRam.bus_out(common_bus);

    std::cout << "Testing Central Bus with Tri-state Logic\n";
    std::cout << "Time | EN_ALU | EN_RAM | Bus Content | Note\n";
    std::cout << "--------------------------------------------------\n";

    // Caso 1: Nadie habla
    en_alu.write(0); en_ram.write(0);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " |   0    |   0    |    " << common_bus.read() << "    | Bus Floating (Hi-Z)\n";

    // Caso 2: Solo ALU habla (envia un 5)
    data_alu.write(5); en_alu.write(1);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " |   1    |   0    |    " << common_bus.read() << "    | ALU controls the bus\n";

    // Caso 3: Solo RAM habla (envia un 9)
    en_alu.write(0); en_ram.write(1); data_ram.write(9);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " |   0    |   1    |    " << common_bus.read() << "    | RAM controls the bus\n";

    // Caso 4: ¡CONFLICTO! (Ambos intentan hablar)
    en_alu.write(1); en_ram.write(1);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " |   1    |   1    |    " << common_bus.read() << "    | ERROR: Bus Contention\n";

    return 0;
}