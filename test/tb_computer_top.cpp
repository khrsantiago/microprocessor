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

    // EL PRIMER APRETON DE MANOS (Inyeccion Manual)
    // LDA = 0001 (1 en Hex), Operando = 0101 (5 en Hex)
    // Instruccion completa: 0x15
    // LDA 5: Desplazamos el OpCode 4 bits a la izquierda y sumamos el Operando
    // Inyectamos un par de instrucciones directo en la ROM
    Computer.Ram->memory[0] = (OP_LDA << 4) | 0x05;
    // La RAM_Out deberia conectarse enseguida al Bus, y luego ser capturada
    // por el Registrador de Instrucciones
    Computer.Ram->memory[1] = (OP_JMP << 4) | 0x00;
    
    // Inyectamos un dato de prueba en la direccion 5
    Computer.Ram->memory[5] = 0x09; // Queremos que este 9 llegue al Acumulador

    std::cout << "Starting 'Handshake' Test (LDA 5)...\n";
    std::cout << "Time  | PC |    IR (Hex)    | MAR | RAM_Out | RegA | Bus  | FSM\n";
    std::cout << "-----------------------------------------------------------\n";



    // Pulso de Reset inicial
    reset.write(1);
    sc_start(15, SC_NS);
    reset.write(0);

    // Escaneamos 6 ciclos de reloj (Los 6 T-States de la instruccion LDA)
    for(int i = 0; i < 6; i++) {
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
    
    // 1. Inyectamos la ROM (El Codigo)
    Computer.Ram->memory[0] = (OP_LDA << 4) | 0x06; // 0: LDA 6
    Computer.Ram->memory[1] = (OP_ADD << 4) | 0x07; // 1: ADD 7
    Computer.Ram->memory[2] = (OP_OUT << 4) | 0x00; // 2: OUT
    Computer.Ram->memory[3] = (OP_JMP << 4) | 0x01; // 3: JMP 1 (Vuelve al ADD)

    // Datos
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