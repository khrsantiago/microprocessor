#include <systemc.h>
#include <iomanip>
#include "computer_top.h"
#include "assembler.h"

int sc_main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Uso: " << argv[0] << " <archivo.asm>\n";
        return 1;
    }

    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    sc_clock clk("main_clock", 10, SC_NS);
    sc_signal<bool> reset;

    ComputerTop Computer("sap_1_computer");
    Computer.clk(clk);
    Computer.reset(reset);  

    // Cargar el programa usando el AssemblerLoader
    if (!AssemblerLoader::load(argv[1], Computer.Ram->memory)) {
        return 1;
    }

    std::cout << "\n--- Iniciando Simulacion de SAP-1 ---\n";
    std::cout << "Archivo: " << argv[1] << "\n";
    std::cout << "Time  | PC |    IR (Hex)    | MAR | RAM_Out | RegA | Bus  | FSM\n";
    std::cout << "-----------------------------------------------------------\n";

    // Pulso de Reset inicial
    reset.write(1);
    sc_start(15, SC_NS);
    reset.write(0);

    // Ejecutar la simulación
    // Le damos un máximo de 200 ciclos o hasta que encuentre un HLT
    bool halted = false;
    for(int i = 0; i < 5000 && !halted; i++) {
        sc_start(10, SC_NS);
        
        // Formatear salida similar a los tests originales
        std::cout << std::setw(5) << sc_time_stamp() << " |  "
                  << Computer.s_pc_to_bus.read() << " |    "
                  << Computer.s_ir_opcode.read() << Computer.s_ir_operand.read() << "    |  "
                  << Computer.s_mar_to_ram.read() << "  |    "
                  << Computer.s_ram_to_bus.read() << "    |  "
                  << Computer.s_rega_out.read() << "   | "
                  << Computer.common_bus.read() << " | T"
                  << (Computer.Cu->current_state.read() + 1) << "\n";

        // Si el OpCode es HLT (0xF) y estamos en el estado final de la instruccion, detenemos
        if (Computer.s_ir_opcode.read().to_uint() == 0xF) {
            halted = true;
            std::cout << "\n[HALT] Procesamiento detenido por instrucción HLT.\n";
        }
    }

    if (!halted) {
        std::cout << "\n[TIMEOUT] La simulación terminó por límite de ciclos.\n";
    }

    return 0;
}
