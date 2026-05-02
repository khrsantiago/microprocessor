#include <systemc.h>
#include "ram.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    sc_clock clk("sys_clk", 10, SC_NS);
    sc_signal<bool> sig_we;
    sc_signal<sc_uint<4>> sig_addr, sig_data_in, sig_data_out;

    RAM Dut("Main_RAM");
    Dut.clk(clk);
    Dut.we(sig_we);
    Dut.addr(sig_addr);
    Dut.data_in(sig_data_in);
    Dut.data_out(sig_data_out);

    std::cout << "Testing 16x4 RAM (Write Sync / Read Async)\n";
    std::cout << "Time  | WE | Address | Data_In | Data_Out | Action\n";
    std::cout << "--------------------------------------------------------\n";

    // 1. Escribir '5' en la Direccion '10'
    sig_we.write(1);
    sig_addr.write(10);
    sig_data_in.write(5);
    sc_start(10, SC_NS); // Esperamos el flanco de subida
    std::cout << sc_time_stamp() << " |  " << sig_we.read() << " |    " << sig_addr.read() 
              << "   |    " << sig_data_in.read() << "    |    " << sig_data_out.read() 
              << "     | Wrote 5 to Addr 10\n";

    // 2. Escribir '9' en la Direccion '0'
    sig_we.write(1);
    sig_addr.write(0);
    sig_data_in.write(9);
    sc_start(10, SC_NS);
    std::cout << sc_time_stamp() << " |  " << sig_we.read() << " |    " << sig_addr.read() 
              << "    |    " << sig_data_in.read() << "    |    " << sig_data_out.read() 
              << "     | Wrote 9 to Addr 0\n";

    // 3. Proteger la memoria (WE = 0) y leer la Direccion 10 asincronamente
    sig_we.write(0);
    sig_addr.write(10);
    sig_data_in.write(0); // Ponemos ruido en la entrada para probar seguridad
    sc_start(5, SC_NS); // Medio ciclo (Lectura asincrona, no requiere reloj)
    std::cout << sc_time_stamp() << " |  " << sig_we.read() << " |    " << sig_addr.read() 
              << "   |    " << sig_data_in.read() << "    |    " << sig_data_out.read() 
              << "     | Read Addr 10 (Protected)\n";

    // 4. Leer la Direccion 0 asincronamente
    sig_addr.write(0);
    sc_start(5, SC_NS);
    std::cout << sc_time_stamp() << " |  " << sig_we.read() << " |    " << sig_addr.read() 
              << "    |    " << sig_data_in.read() << "    |    " << sig_data_out.read() 
              << "     | Read Addr 0 (Protected)\n";

    return 0;
}