#include <systemc.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

#include "computer_top.h"
#include "assembler.h"
#include "isa_helper.h"

// =========================================================================
// SCREEN VISUALIZER (Motor Gráfico Dedicado)
// Únicamente dibuja el bloque 5x5 de VRAM (Direcciones 231-255)
// Sin output de trazados locales, 100% cinemático.
// =========================================================================

int sc_main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <archivo.asm> [--delay=X]" << std::endl;
        return 1;
    }

    int delay_ms = 100; // FPS (100ms default)
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.find("--delay=") == 0) {
            delay_ms = std::stoi(arg.substr(8));
        }
    }

    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);

    sc_clock sys_clk("sys_clk", 10, SC_NS);
    sc_signal<bool> reset_sg;

    ComputerTop Computer("KHR8_COMPUTER");
    Computer.clk(sys_clk);
    Computer.reset(reset_sg);

    RAM* ram = Computer.DataRam;

    if (!AssemblerLoader::load(argv[1], ram->memory) || !AssemblerLoader::load(argv[1], Computer.InstRom->memory)) {
        std::cerr << "Fallo al inicializar la RAM desde el ensamblador." << std::endl;
        return 1;
    }

    bool halted = false;
    int frame = 0;
    bool rendered_this_out = false;

    // Reset de Hardware inicial (Sincronización de Fase)
    reset_sg.write(true);
    sc_start(20, SC_NS);
    reset_sg.write(false);

    // Primer Clear de Consola
    std::cout << "\033[2J\033[H";

    // Loop expandido a 20M Ticks
    for(unsigned long i = 0; i < 20000000 && !halted; i++) {
        sc_start(10, SC_NS);
        
        // int state = 7; // Simulando pipeline siempre en ejecucion
        unsigned char opcode = Computer.s_ir_opcode.read().to_uint();

        if (opcode == OP_HLT) {
            halted = true;
            break;
        }

        // Reset flag at the start of each new instruction fetch (T1)
        if (Computer.s_pc_en.read() == 1) rendered_this_out = false;

        // Evento VSYNC dedicado: Si dispara instrucción OUT (R0-R3: 0xE0-0xE3)
        if ((opcode >= 0xE0 && opcode <= 0xE3) && !rendered_this_out) {
            rendered_this_out = true;
            frame++;
            std::cout << "\033[2J\033[H"; // Clear screen and set cursor to Top-Left
            
            std::cout << IsAHelper::CYAN << "===============================================================" << IsAHelper::RESET << std::endl;
            std::cout << IsAHelper::BOLD << "           CONWAY'S GAME OF LIFE (5x5 VRAM ENGINE)" << IsAHelper::RESET << std::endl;
            std::cout << IsAHelper::CYAN << "===============================================================" << IsAHelper::RESET << std::endl;
            
            for (int y = 0; y < 5; ++y) {
                std::cout << "                      | ";
                for (int x = 0; x < 5; ++x) {
                    int val = ram->memory[231 + y*5 + x];
                    if (val > 0) std::cout << IsAHelper::GREEN << "\u2588 " << IsAHelper::RESET;
                    else std::cout << "\u2591 ";
                }
                std::cout << "|" << std::endl;
            }
            std::cout << IsAHelper::CYAN << "\n===============================================================" << IsAHelper::RESET << std::endl;
            std::cout << IsAHelper::YELLOW << "  Frame: " << frame << IsAHelper::RESET << std::endl;

            // Retraso interactivo visual del fotograma (FPS Cap)
            if (delay_ms > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
            }
        }
    }

    std::cout << IsAHelper::RED << IsAHelper::BOLD << "\n SIMULACIÓN DETENIDA." << IsAHelper::RESET << std::endl;
    return 0;
}
