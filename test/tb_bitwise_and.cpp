#include <systemc.h>
#include "bitwise_and.h"

int sc_main(int argc, char* argv[]) {
    // Apagamos los mensajes del sistema para tener una consola limpia
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    // Definición de cables virtuales
    sc_signal<sc_uint<4>> sig_a, sig_b, sig_result;

    // Instanciamos el Dispositivo Bajo Prueba (DUT - Device Under Test)
    BitwiseAnd Dut("dut_bitwise_and");
    Dut.a(sig_a);
    Dut.b(sig_b);
    Dut.result(sig_result);

    std::cout << "Logical Unit: AND (4-bit)\n";
    std::cout << "Time  |    A    |    B    ||  Result\n";
    std::cout << "--------------------------------------\n";

    // Caso 1: Filtro de bits (Masking)
    // 1111 (15) AND 1010 (10) -> Solo pasan los unos de B
    sig_a.write(15); sig_b.write(10);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " | " << sig_a.read().to_string(SC_BIN) 
              << " | " << sig_b.read().to_string(SC_BIN) 
              << " || " << sig_result.read().to_string(SC_BIN) << "\n";

    // Caso 2: Opuestos excluyentes
    // 1100 (12) AND 0011 (3) -> Ningún 1 coincide en la misma posición
    sig_a.write(12); sig_b.write(3);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " | " << sig_a.read().to_string(SC_BIN) 
              << " | " << sig_b.read().to_string(SC_BIN) 
              << " || " << sig_result.read().to_string(SC_BIN) << "\n";

    // Caso 3: Idénticos
    // 0101 (5) AND 0101 (5) -> El número se mantiene igual
    sig_a.write(5); sig_b.write(5);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " | " << sig_a.read().to_string(SC_BIN) 
              << " | " << sig_b.read().to_string(SC_BIN) 
              << " || " << sig_result.read().to_string(SC_BIN) << "\n";

    return 0;
}