#include <systemc.h>
#include <iostream>
#include <cassert>
#include "computer_top.h"
#include "assembler.h"

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    sc_clock clk("main_clock", 10, SC_NS);
    sc_signal<bool> reset;

    ComputerTop Computer("KHR_8_REGRESSION_TEST");
    Computer.clk(clk);
    Computer.reset(reset);  

    std::cout << "[REGRESIÓN] Evaluando KHR-8 Pipeline con Datapath Forwarding...\n";

    // 1. Cargamos el programa base desde disco duro
    if (!AssemblerLoader::load("asm/program.asm", Computer.DataRam->memory) || 
        !AssemblerLoader::load("asm/program.asm", Computer.InstRom->memory)) {
        std::cerr << "CRITICAL ERROR: No se puedo cargar asm/program.asm para el test de regresión." << std::endl;
        return 1;
    }

    // 2. Reinicio de sistema
    reset.write(1);
    sc_start(15, SC_NS);
    reset.write(0);

    // 3. Dejamos que corra el programa libremente para que opere la suma (7 + 3)
    bool halted = false;
    for(int i = 0; i < 50 && !halted; i++) {
        sc_start(10, SC_NS);
        if (Computer.s_ir_opcode.read().to_uint() == 0xF) { // Halt
            halted = true;
            sc_start(30, SC_NS); // Drenar Pipeline
        }
    }

    // 4. Afirmaciones Cruciales (Regresiones)
    int final_out = Computer.OutReg->display.read();
    int mem_15_val = Computer.DataRam->memory[15];

    std::cout << "\nRESULTADOS FINALES DE LA MAQUINA:" << std::endl;
    std::cout << " > Output Display = " << final_out << " (Esperado: 10)" << std::endl;
    std::cout << " > Memory Addr 15 = " << mem_15_val << " (Esperado: 7)" << std::endl;

    if (final_out == 10 && mem_15_val == 7) {
        std::cout << "\n[PASS] Todos los pipelines y cortocircuitos pasaron exitosamente. Cero riesgos de datos (Hazards).\n" << std::endl;
        return 0; // Success
    } else {
        std::cout << "\n[FAIL] Regresión Fallida. Se detectó un rompedero en el procesamiento concurrente o en la evaluación ALU.\n" << std::endl;
        return 1; // Error
    }
}
