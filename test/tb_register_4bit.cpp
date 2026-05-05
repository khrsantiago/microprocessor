#include <systemc.h>
#include "register_4bit.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    // 1. Instanciamos un Reloj del Sistema
    // Nombre: "sys_clk", Periodo: 10 nanosegundos (5ns en 1, 5ns en 0)
    sc_clock clk("sys_clk", 10, SC_NS);

    // Cables virtuales
    sc_signal<sc_uint<8>> sig_d, sig_q;

    // 2. Instanciamos el Registro
    Register8Bit Dut("Main_Register");
    Dut.clk(clk); 
    Dut.d(sig_d);
    Dut.q(sig_q);

    std::cout << "Sequential Logic: 8-Bit Register (D Flip-Flop)\n";
    std::cout << "Time  | D (In) | Q (Out) | Description\n";
    std::cout << "------------------------------------------\n";

    // 1. Inicializamos en 0
    sig_d.write(0);
    sc_start(2, SC_NS); // Avanzamos a t=2ns
    std::cout << sc_time_stamp() << "  |   " << sig_d.read() << "    |    " << sig_q.read() << "    | Start.\n";

    // 2. Cambiamos D a la mitad del ciclo (t=5ns). No hay flanco aqui.
    sig_d.write(7);
    sc_start(3, SC_NS); // Avanzamos 3ns mas (t=5ns total)
    std::cout << sc_time_stamp() << "  |   " << sig_d.read() << "    |    " << sig_q.read() << "    | D changes to 7. Q is NOT updated.\n";

    // 3. Dejamos que pase el flanco de subida de los 10ns
    // 3. Let the 10ns rising edge pass
    sc_start(6, SC_NS); // Advance 6ns more (t=11ns total)
    std::cout << sc_time_stamp() << " |   " << sig_d.read() << "    |    " << sig_q.read() << "    | 10ns Edge! Q captured 7.\n";

    // 4. Inject noise by changing D at mid-cycle
    sig_d.write(12);
    sc_start(4, SC_NS); // Advance 4ns more (t=15ns total)
    std::cout << sc_time_stamp() << " |   " << sig_d.read() << "   |    " << sig_q.read() << "    | D changes to 12. Q protects 7!\n";

    // 5. Let the 20ns rising edge pass
    sc_start(6, SC_NS); // Advance 6ns more (t=21ns total)
    std::cout << sc_time_stamp() << " |   " << sig_d.read() << "   |   " << sig_q.read() << "    | 20ns Edge! Q updates to 12.\n";
    return 0;
}