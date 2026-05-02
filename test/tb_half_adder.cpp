#include <systemc.h>
#include "half_adder.h"

int sc_main(int argc, char* argv[]) {
    // Apaga todos los mensajes informativos internos de SystemC
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    // 1. Declaramos los "cables" para conectar nuestro chip
    sc_signal<bool> sig_a, sig_b;      // Cables de entrada
    sc_signal<bool> sig_sum, sig_cout; // Cables de salida

    // 2. Instanciamos el Dispositivo Bajo Prueba (DUT)
    HalfAdder Dut("half_adder_0");
    
    // Conectamos los pines del chip a nuestros cables
    Dut.a(sig_a);
    Dut.b(sig_b);
    Dut.sum(sig_sum);
    Dut.cout(sig_cout);

    // [OPCIONAL PERO MUY PRO] Crear un archivo de trazas para ver las ondas electromagnéticas
    sc_trace_file *wf = sc_create_vcd_trace_file("wave_half_adder");
    // Le decimos a SystemC que la resolución de la gráfica será de 1 Nanosegundo
    wf->set_time_unit(1, SC_NS);

    sc_trace(wf, sig_a, "A");
    sc_trace(wf, sig_b, "B");
    sc_trace(wf, sig_sum, "Adder");
    sc_trace(wf, sig_cout, "Carry Out");

    // 3. Inyectamos los estímulos (Las pruebas unitarias)
    std::cout << "Starting Half Adder test...\n";
    std::cout << "Time | A | B || Sum | Cout\n";
    std::cout << "---------------------------------\n";

    // Prueba 1: 0 + 0
    sig_a.write(0); 
    sig_b.write(0);
    sc_start(10, SC_NS); // Avanzamos el reloj de simulación 10 nanosegundos
    std::cout << sc_time_stamp() << "  | " 
              << sig_a.read() << " | " 
              << sig_b.read() << " |  " 
              << sig_sum.read() << "   |   " 
              << sig_cout.read() << "\n";

    // Prueba 2: 0 + 1
    sig_a.write(0); 
    sig_b.write(1);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << "  | " 
              << sig_a.read() << " | " 
              << sig_b.read() << " |  " 
              << sig_sum.read() << "   |   " 
              << sig_cout.read() << "\n";

    // Prueba 3: 1 + 0
    sig_a.write(1); 
    sig_b.write(0);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << "  | " 
              << sig_a.read() << " | " 
              << sig_b.read() << " |  " 
              << sig_sum.read() << "   |   " 
              << sig_cout.read() << "\n";

    // Prueba 4: 1 + 1 (Debería dar Suma=0, Acarreo=1)
    sig_a.write(1); 
    sig_b.write(1);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << "  | " 
              << sig_a.read() << " | " 
              << sig_b.read() << " |  " 
              << sig_sum.read() << "   |   " 
              << sig_cout.read() << "\n";

    // Cerramos el archivo de ondas
    sc_close_vcd_trace_file(wf);

    return 0;
}