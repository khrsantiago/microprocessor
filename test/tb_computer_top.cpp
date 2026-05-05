#include <systemc.h>
#include <iomanip>
#include "computer_top.h"
#include "isa.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    sc_clock clk("main_clock", 10, SC_NS);
    sc_signal<bool> reset;

    ComputerTop Computer("sap_1_computer");
    Computer.clk(clk);
    Computer.reset(reset);  

    // LDA 5: OpCode en mem[0], Operando en mem[1]
    Computer.Ram->memory[0] = OP_LDA;
    Computer.Ram->memory[1] = 5;
    
    // JMP 0: OpCode en mem[2], Operando en mem[3]
    Computer.Ram->memory[2] = OP_JMP;
    Computer.Ram->memory[3] = 0;
    
    // Inyectamos un dato de prueba en la direccion 5
    Computer.Ram->memory[5] = 0x09; // Queremos que este 9 llegue al Acumulador

    std::cout << "Starting 'Handshake' Test (LDA 5)...\n";
    std::cout << "Time  | PC |    IR (Hex)    | MAR | RAM_Out | RegA | Bus  | FSM\n";
    std::cout << "-----------------------------------------------------------\n";



    // Pulso de Reset inicial
    reset.write(1);
    sc_start(15, SC_NS);
    reset.write(0);

    // Escaneamos 12 ciclos de reloj (Los 12 T-States de la FSM extendida)
    for(int i = 0; i < 12; i++) {
        sc_start(10, SC_NS);
        
        
        std::cout << std::setw(5) << sc_time_stamp() << " |  "
                  << Computer.s_pc_to_bus.read() << " |    "
                  // Imprimimos el OpCode y el Operando juntos
                  << Computer.s_ir_opcode.read() << Computer.s_ir_operand.read() << "    |  "
                  << Computer.s_mar_to_ram.read() << "  |    "
                  << Computer.s_ram_to_bus.read() << "    |  "
                  << Computer.s_rega_out.read() << "   | "
                  << Computer.common_bus.read() << " | T"
                  // Convertimos el enum del estado a numero (T1=1, T2=2...)
                  << (Computer.Cu->current_state.read() + 1) << "\n";
    }

    

/*
    // Dejamos que la maquina corra 2 ciclos completos de Fetch/Execute
    // (12 T-States = 120 ns)
    sc_start(120, SC_NS);

    std::cout << "Simulation complete. Check VCD traces for signal mapping.\n";
    */

    // =================================================================
    // EL PROGRAMA MAESTRO: Bucle Infinito de Conteo (Hello World)
    // =================================================================
    
    // 1. Inyectamos la ROM (El Codigo de 16 bits)
    Computer.Ram->memory[0] = OP_LDA; Computer.Ram->memory[1] = 6; // 0: LDA 6
    Computer.Ram->memory[2] = OP_ADD; Computer.Ram->memory[3] = 7; // 2: ADD 7
    Computer.Ram->memory[4] = OP_OUT; Computer.Ram->memory[5] = 0; // 4: OUT
    Computer.Ram->memory[6] = OP_JMP; Computer.Ram->memory[7] = 2; // 6: JMP 2 (Vuelve al ADD)

    // Datos (Ahora en direcciones seguras)
    Computer.Ram->memory[6] = 0x00; // Valor inicial
    Computer.Ram->memory[7] = 0x01; // Incremento (+1)

    // 3. ¡EL REINICIO VITAL! (Apagar y encender la maquina)
    reset.write(1);
    sc_start(15, SC_NS);
    reset.write(0);

    std::cout << "\nStarting 'Hello World' Counting Loop...\n";
    std::cout << "Time  | PC |    IR (Hex)    | MAR | RAM_Out | RegA | Bus  | FSM\n";
    std::cout << "-----------------------------------------------------------\n";

    // 4. Ejecutar y Observar
    // Le damos 40 ciclos para asegurar que el bucle de la muerte de la vuelta
    // al menos dos o tres veces para ver el contador subir.
    for(int i = 0; i < 40; i++) { 
        sc_start(10, SC_NS);
        
        std::cout << std::setw(5) << sc_time_stamp() << " |  "
                  << Computer.s_pc_to_bus.read() << " |    "
                  << Computer.s_ir_opcode.read() << Computer.s_ir_operand.read() << "    |  "
                  << Computer.s_mar_to_ram.read() << "  |    "
                  << Computer.s_ram_to_bus.read() << "    |  "
                  << Computer.s_rega_out.read() << "   | "
                  << Computer.common_bus.read() << " | T"
                  << (Computer.Cu->current_state.read() + 1) << "\n";
    }

    return 0;
}