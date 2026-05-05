#include <systemc.h>
#include <iomanip>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include "computer_top.h"
#include "assembler.h"
#include "isa_helper.h"

// Clase para manejar el historial de ejecución
struct TraceEntry {
    int addr;
    std::string mnemonic;
    int operand;
    std::string result;
};

std::vector<TraceEntry> g_history;
std::string g_last_output = "N/A";
int g_delay_ms = 250; // Intervalo interactivo predeterminado

void render_dashboard(ComputerTop& comp, bool live_mode) {
    if (!live_mode) return;

    // Limpiar pantalla y mover cursor al inicio
    std::cout << "\033[2J\033[H";

    std::cout << IsAHelper::BOLD << IsAHelper::CYAN 
              << "===============================================================" << std::endl;
    std::cout << "             SAP-1 INTERACTIVE DASHBOARD (8-Bit)" << std::endl;
    std::cout << "===============================================================" << IsAHelper::RESET << std::endl;

    // 1. Panel de Registros (Hardware)
    unsigned int pc = comp.s_pc_to_bus.read().to_uint();
    unsigned char regA = comp.s_rega_out.read().to_uint();
    unsigned char regX = comp.s_regx_out.read().to_uint();
    int state = comp.Cu->current_state.read() + 1;
    bool z_flag = comp.s_alu_zero_flag.read();

    std::cout << IsAHelper::BOLD << " REGISTROS: " << IsAHelper::RESET;
    std::cout << "[PC: 0x" << std::hex << std::setw(2) << std::setfill('0') << pc << std::dec << std::setfill(' ') << "] ";
    std::cout << "[RegA: " << std::dec << (int)regA << "] ";
    std::cout << "[RegX: " << std::dec << (int)regX << "] ";
    std::cout << "[Zero: " << (z_flag ? "TRUE" : "FALSE") << "] ";
    std::cout << "[Ciclo: T" << state << "]" << std::endl;
    
    std::cout << IsAHelper::CYAN << "---------------------------------------------------------------" << IsAHelper::RESET << std::endl;

    // 2. Historial de Ejecución (Software)
    std::cout << IsAHelper::YELLOW << " HISTORIAL RECIENTE:" << IsAHelper::RESET << std::endl;
    int start = (g_history.size() > 5) ? g_history.size() - 5 : 0;
    for (size_t i = start; i < g_history.size(); ++i) {
        bool is_last = (i == g_history.size() - 1);
        std::cout << (is_last ? "  > " : "    ") 
                  << "0x" << std::hex << std::setw(2) << std::setfill('0') << g_history[i].addr 
                  << std::dec << std::setfill(' ') << ": "
                  << std::left << std::setw(4) << g_history[i].mnemonic 
                  << std::setw(3) << g_history[i].operand;
        if (!g_history[i].result.empty()) {
            std::cout << IsAHelper::CYAN << " │ " << g_history[i].result << IsAHelper::RESET;
        }
        std::cout << std::endl;
    }
    
    for (size_t i = (g_history.size() - start); i < 5; ++i) std::cout << std::endl; // Padding

    std::cout << IsAHelper::CYAN << "---------------------------------------------------------------" << IsAHelper::RESET << std::endl;

    // 3. Salida de Pantalla
    std::cout << IsAHelper::GREEN << " ÚLTIMA SALIDA (OUT): " << IsAHelper::BOLD 
              << g_last_output << IsAHelper::RESET << std::endl;
    std::cout << IsAHelper::CYAN << "===============================================================" << IsAHelper::RESET << std::endl;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(g_delay_ms)); // Paso visible
}

int sc_main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Uso: " << argv[0] << " <archivo.asm> [--live]\n";
        return 1;
    }

    bool live_mode = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--live") live_mode = true;
        else if (arg == "--fast") g_delay_ms = 20; // Modalidad Turbo
        else if (arg.find("--delay=") == 0) g_delay_ms = std::stoi(arg.substr(8));
    }

    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);
    sc_clock clk("main_clock", 10, SC_NS);
    sc_signal<bool> reset;

    ComputerTop Computer("sap_1_computer");
    Computer.clk(clk);
    Computer.reset(reset);  

    if (!AssemblerLoader::load(argv[1], Computer.Ram->memory)) {
        return 1;
    }

    if (!live_mode) {
        std::cout << IsAHelper::BOLD << IsAHelper::CYAN 
                << "\n==============================================" << std::endl;
        std::cout << "   SAP-1 HUMAN VISUALIZER v2.1 (8-Bit)" << std::endl;
        std::cout << "==============================================" << IsAHelper::RESET << std::endl;
        std::cout << "Archivo: " << IsAHelper::YELLOW << argv[1] << IsAHelper::RESET << "\n" << std::endl;
    }

    reset.write(1);
    sc_start(15, SC_NS);
    reset.write(0);

    bool halted = false;

    // Aumentamos gigantescamente el límite para sostener loops anidados (20 Millones en lugar de 20 Mil)
    for(unsigned long i = 0; i < 20000000 && !halted; i++) {
        sc_start(10, SC_NS);
        
        int state = Computer.Cu->current_state.read() + 1;
        unsigned int pc = Computer.s_pc_to_bus.read().to_uint();
        unsigned char opcode = Computer.s_ir_opcode.read().to_uint();
        unsigned char operand = Computer.s_ir_operand.read().to_uint();

        // 1. Capturar Nueva Instrucción (T7)
        if (state == 7) {
            int display_addr = (int)pc - 2;
            if (display_addr < 0) display_addr = 0;

            TraceEntry entry;
            entry.addr = display_addr;
            entry.mnemonic = IsAHelper::get_mnemonic(opcode);
            entry.operand = (int)operand;
            g_history.push_back(entry);

            if (!live_mode) {
                std::cout << IsAHelper::BLUE << "[PASO] " << IsAHelper::RESET
                        << "Dirección 0x" << std::hex << std::setw(2) << std::setfill('0') << display_addr 
                        << std::dec << std::setfill(' ') << ": " << IsAHelper::BOLD << entry.mnemonic 
                        << " " << entry.operand << IsAHelper::RESET << std::endl;
            }
            
            if (opcode == 0 && display_addr > 128) halted = true; // Safety IDLE stop
        }

        // 2. Registrar Resultados (T12)
        if (state == 12 && !g_history.empty()) {
            std::string res = "";
            if (opcode == OP_LDA || opcode == OP_LDI || opcode == OP_ADD || opcode == OP_SUB || opcode == OP_TXA || opcode == OP_LDA_X) {
                res = "RegA -> " + std::to_string(Computer.s_rega_out.read().to_uint());
            } else if (opcode == OP_TAX) {
                res = "RegX -> " + std::to_string(Computer.s_regx_out.read().to_uint());
            } else if (opcode == OP_STA) {
                res = "Mem[" + std::to_string(operand) + "] <- " + std::to_string(Computer.s_rega_out.read().to_uint());
            } else if (opcode == OP_STA_X) {
                res = "Mem[X(" + std::to_string(Computer.s_regx_out.read().to_uint()) + ")] <- " + std::to_string(Computer.s_rega_out.read().to_uint());
            } else if (opcode == OP_JMP || (opcode == OP_JZ && Computer.s_alu_zero_flag.read()) || (opcode == OP_JC && Computer.s_latched_carry.read())) {
                res = "SALTO!";
            }
            g_history.back().result = res;

            if (!live_mode && !res.empty()) {
                std::cout << "       " << IsAHelper::CYAN << "└─> " << res << IsAHelper::RESET << std::endl;
            }
        }

        // 3. Salida a Consola (T8)
        if (opcode == OP_OUT && state == 8) {
            g_last_output = std::to_string(Computer.s_rega_out.read().to_uint());
            if (!live_mode) {
                std::cout << IsAHelper::YELLOW << IsAHelper::BOLD 
                        << "\n[SALIDA] >>> " << g_last_output << " <<<\n" << IsAHelper::RESET << std::endl;
            }
        }

        // 4. Parada (HLT)
        if (opcode == OP_HLT && state == 7) {
            halted = true;
            if (!live_mode) {
                std::cout << "\n" << IsAHelper::RED << IsAHelper::BOLD 
                        << "[FIN] Programa terminado por instrucción HLT." << IsAHelper::RESET << std::endl;
            }
        }

        // 5. Refrescar Dashboard si está activo
        if (live_mode && (state == 1 || state == 7 || state == 12)) {
            render_dashboard(Computer, live_mode);
        }
    }

    if (live_mode) {
        render_dashboard(Computer, live_mode);
        std::cout << IsAHelper::RED << IsAHelper::BOLD << "\n SIMULACIÓN FINALIZADA." << IsAHelper::RESET << std::endl;
    } else if (!halted) {
        std::cout << IsAHelper::RED << "\n[INFO] Simulación finalizada por límite de tiempo." << IsAHelper::RESET << std::endl;
    }

    return 0;
}
