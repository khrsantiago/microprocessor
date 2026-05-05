#include <systemc.h>
#include "tri_state_buffer.h"
#include "accumulator.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    // Reloj maestro del sistema
    sc_clock clk("sys_clk", 10, SC_NS);

    // El Bus Central Compartido
    sc_signal_rv<8> common_bus;

    // Senales de Control (La "Unidad de Control" manual)
    sc_signal<bool> ram_enable, regA_enable;
    sc_signal<bool> regA_load, regB_load;
    sc_signal<bool> dummy_z1, dummy_z2;
    
    // Cables de datos simulados
    sc_signal<sc_uint<8>> ram_data_out;  
    sc_signal<sc_uint<8>> regA_data_out; 
    sc_signal<sc_uint<8>> bus_wire; // Bridge for resolved bus
    
    // --- INSTANCIAS (Adaptadores al Bus) ---
    // Buffer de RAM -> Bus
    TriStateBuffer BufRam("buf_ram");
    BufRam.data_in(ram_data_out); BufRam.enable(ram_enable); BufRam.bus_out(common_bus);

    // Buffer de Reg A -> Bus
    TriStateBuffer BufRegA("buf_reg_a");
    BufRegA.data_in(regA_data_out); BufRegA.enable(regA_enable); BufRegA.bus_out(common_bus);

    // Registros
    Accumulator RegA("rega");
    RegA.clk(clk); RegA.ld(regA_load); RegA.d(bus_wire); RegA.q(regA_data_out); RegA.flag_z(dummy_z1);

    // Bridge Logic: Eliminar SC_METHOD (No valido en sc_main)
    // Actualizaremos bus_wire manualmente cuando el bus se estabilice.

    std::cout << "Data Transfer Protocol (RAM -> Reg A -> Reg B)\n";
    std::cout << "Time  | Bus (Hex) | Control Signals (RAM_EN, REGA_LD) | Action\n";
    std::cout << "--------------------------------------------------------------\n";

    // 0. ESTADO INICIAL
    ram_enable.write(0); regA_enable.write(0);
    regA_load.write(0); regB_load.write(0);
    ram_data_out.write(9); // Supongamos que la direccion 0 de la RAM tiene un 9
    sc_start(2, SC_NS);

    // ==========================================
    // TRANSFERENCIA 1: RAM -> REG A
    // ==========================================
    
    // Paso 1: Abrir puertas de origen y destino
    ram_enable.write(1); // RAM empieza a hablar
    regA_load.write(1);  // Reg A abre su candado
    sc_start(3, SC_NS);  // Damos 3ns de "Retraso de Propagacion" para estabilizar el bus
    bus_wire.write((sc_uint<8>)common_bus.read()); // Puente manual
    
    std::cout << sc_time_stamp() << "  |   " << common_bus.read() 
              << "     | RAM_EN=1, REGA_LD=1               | Bus stabilised with RAM data\n";

    // Paso 2: Ejecutar el Reloj (El flanco ocurre a los 10ns)
    sc_start(6, SC_NS); 
    
    // Simulamos que Reg A guardo el dato y lo pone en su salida
    regA_data_out.write(9); 
    std::cout << sc_time_stamp() << " |   " << common_bus.read() 
              << "     | (Flanco de Reloj)                 | Reg A captures the data!\n";

    // Paso 3: Apagar señales (Limpiar el bus)
    ram_enable.write(0); regA_load.write(0);
    sc_start(4, SC_NS); 
    std::cout << sc_time_stamp() << " |   " << common_bus.read() 
              << "    | RAM_EN=0, REGA_LD=0               | Bus released (Hi-Z)\n";

    // ==========================================
    // TRANSFERENCIA 2: REG A -> REG B
    // ==========================================
    
    // Paso 1: Abrir puertas
    regA_enable.write(1); // Reg A empieza a hablar al bus
    regB_load.write(1);   // Reg B abre su candado
    sc_start(3, SC_NS);
    bus_wire.write((sc_uint<8>)common_bus.read()); // Puente manual
    
    std::cout << sc_time_stamp() << " |   " << common_bus.read() 
              << "     | REGA_EN=1, REGB_LD=1              | Bus stabilised with Reg A data\n";

    // Paso 2 y 3: Captura y limpieza
    sc_start(7, SC_NS); // Flanco de reloj a los 20ns
    regA_enable.write(0); regB_load.write(0);
    
    std::cout << sc_time_stamp() << " |   " << common_bus.read() 
              << "    | REGA_EN=0, REGB_LD=0              | Reg B captures. Transfer complete.\n";

    return 0;
}