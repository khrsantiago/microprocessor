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
// SCREEN 16x16 VISUALIZER (Motor Gráfico 16-bits)
// Resolución: 16x16 píxeles (VRAM Dedicada: 0x0100-0x01FF)
// Soporte de Color: RGB565
// =========================================================================

void print_pixel(uint16_t c) {
    if (c == 0) {
        std::cout << "\033[90m\u2591 "; // Gris oscuro (vacio)
    } else {
        // Siempre decodificar como RGB565
        int r = ((c >> 11) & 0x1F) << 3;
        int g = ((c >> 5)  & 0x3F) << 2;
        int b = ( c        & 0x1F) << 3;
        // Si el color es muy oscuro (< umbral), forzar blanco brillante
        if (r + g + b < 30) {
            std::cout << "\033[97m\u2588 ";
        } else {
            std::cout << "\033[38;2;" << r << ";" << g << ";" << b << "m\u2588 ";
        }
    }
    std::cout << "\033[0m";
}

int sc_main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <archivo.asm> [--delay=X]" << std::endl;
        return 1;
    }

    int delay_ms = 50; 
    bool is_manual_mode = false;
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.find("--delay=") == 0) delay_ms = std::stoi(arg.substr(8));
        else if (arg == "--manual") is_manual_mode = true;
    }

    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);
    sc_clock sys_clk("sys_clk", 10, SC_NS);
    sc_signal<bool> reset_sg;

    ComputerTop Computer("KHR8_16_GRAPHICS");
    Computer.clk(sys_clk);
    Computer.reset(reset_sg);
    Computer.OutReg->enable_console_output = false;

    // Inicializar RAM y VRAM a 0
    for(int i=0; i<256; i++) {
        Computer.DataRam->memory[i] = 0;
        Computer.DataVram->memory[i] = 0;
    }
 
    if (!AssemblerLoader::load(argv[1], Computer.InstRom->memory)) {
        std::cerr << "Fallo al inicializar la ROM." << std::endl;
        return 1;
    }

    bool halted = false;
    int frame = 0;
    bool rendered_this_out = false;

    reset_sg.write(true);
    sc_start(20, SC_NS);
    reset_sg.write(false);

    std::cout << "\033[2J\033[H";

    for(unsigned long i = 0; i < 20000000 && !halted; i++) {
        sc_start(10, SC_NS);
        
        uint8_t opcode = Computer.s_wb_opcode.read().to_uint(); // Usamos WB para detectar fin de instrucción

        if (opcode == OP_HLT) {
            halted = true;
            // Un último render para ver el resultado final
            rendered_this_out = false;
        }

        // Detectar instrucción OUT (cualquiera de los 4 registros)
        if ((opcode >= 0xE0 && opcode <= 0xE3) && !rendered_this_out) {
            rendered_this_out = true;
            frame++;
            
            std::cout << "\033[H"; 
            std::cout << IsAHelper::BOLD << IsAHelper::CYAN << "      KHR-8 16-BIT GPU ENGINE (16x16 Dedicada)" << IsAHelper::RESET << std::endl;
            std::cout << "  " << std::string(42, '-') << std::endl;
            
            for (int y = 0; y < 16; ++y) {
                std::cout << "  | ";
                for (int x = 0; x < 16; ++x) {
                    uint16_t pixel = Computer.DataVram->memory[y * 16 + x].to_uint();
                    print_pixel(pixel);
                }
                std::cout << "\033[0m |" << std::endl;
            }
            std::cout << "  " << std::string(42, '-') << std::endl;
            std::cout << IsAHelper::YELLOW << "  Frame: " << std::setw(5) << frame << " | Cycle: " << i << IsAHelper::RESET << std::endl;

            if (is_manual_mode) {
                std::cout << "  Presiona [ENTER]...";
                std::cin.get();
            } else if (delay_ms > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
            }
        }
        

        // Reset flag cuando el PC avanza
        if (Computer.s_pc_en.read() == 1) rendered_this_out = false;
    }

    std::cout << IsAHelper::RED << IsAHelper::BOLD << "\n SIMULACIÓN DETENIDA." << IsAHelper::RESET << std::endl;
    return 0;
}
