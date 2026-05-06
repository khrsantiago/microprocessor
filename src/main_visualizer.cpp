#include <systemc.h>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <limits>
#include "computer_top.h"
#include "assembler.h"
#include "isa_helper.h"

struct TraceEntry {
    int addr;
    std::string full_text;
    int opcode;
    int operand;
    std::string result;
    int ticks_elapsed;
    bool logged;
};

std::vector<TraceEntry> g_history;
std::string g_last_output = "N/A";
int g_delay_ms = 250; 

void render_dashboard(ComputerTop& comp, bool live_mode) {
    if (live_mode) {
        // Limpiar pantalla y mover cursor al inicio
        std::cout << "\033[2J\033[H" << std::flush;
    }

    std::cout << IsAHelper::BOLD << IsAHelper::CYAN 
              << "╔═══════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                     KHR-8 LIVE PIPELINE VISUALIZER                        ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════╝" << IsAHelper::RESET << std::endl;

    // 1. Panel de Registros (Interpretado como signed 8-bit)
    signed char r0 = (signed char)comp.s_rf_r0.read().to_uint();
    signed char r1 = (signed char)comp.s_rf_r1.read().to_uint();
    signed char r2 = (signed char)comp.s_rf_r2.read().to_uint();
    signed char r3 = (signed char)comp.s_rf_r3.read().to_uint();
    bool z_flag = comp.s_flags_z.read();
    bool c_flag = comp.s_flags_c.read();
    bool n_flag = comp.s_flags_n.read();
    bool v_flag = comp.s_flags_o.read();
    unsigned char mar = comp.s_mar_to_ram.read().to_uint();
    unsigned char ram = comp.s_ram_to_bus.read().to_uint();

    std::cout << IsAHelper::BOLD << "  SYSTEM STATE: " << IsAHelper::RESET;
    std::cout << "[R0:" << std::setw(4) << (int)r0 << "] [R1:" << std::setw(4) << (int)r1 << "] [R2:" << std::setw(4) << (int)r2 << "] [R3:" << std::setw(4) << (int)r3 << "] " << std::endl;
    std::cout << "  " << IsAHelper::CYAN << std::string(16, '.') << IsAHelper::RESET;
    std::cout << " [Flags Z:" << z_flag << " C:" << c_flag << " N:" << n_flag << " V:" << v_flag << "] ";
    std::cout << "[MAR:" << std::setw(2) << (int)mar << "] [RAM:" << std::setw(3) << (int)ram << "] ";
    std::cout << "[Bus:" << std::setw(3) << (int)comp.common_bus.read().to_uint() << "]" << std::endl;
    std::cout << IsAHelper::CYAN << "  " << std::string(75, '-') << IsAHelper::RESET << std::endl;

    // 2. Pipeline Stages (Reading directly from hardware for perfect sync)
    struct Stage {
        std::string name;
        std::string color;
        int pc;
        int opcode;
        int operand;
    };

    std::vector<Stage> stages = {
        {"FETCH  (IF)", IsAHelper::BLUE,   (int)comp.s_if_pc_dbg.read().to_uint(), (int)comp.s_if_opcode_dbg.read().to_uint(), (int)comp.s_if_operand_dbg.read().to_uint()},
        {"DECODE (ID)", IsAHelper::YELLOW, (int)comp.s_id_pc_dbg.read().to_uint(), (int)comp.s_id_opcode.read().to_uint(),     (int)comp.s_id_operand.read().to_uint()},
        {"EXEC   (EX)", IsAHelper::CYAN,   (int)comp.s_ex_pc_dbg.read().to_uint(), (int)comp.s_ex_opcode_dbg.read().to_uint(), (int)comp.s_ex_operand_dbg.read().to_uint()},
        {"WBACK  (WB)", IsAHelper::GREEN,  (int)comp.s_wb_pc_dbg.read().to_uint(), (int)comp.s_wb_opcode_dbg.read().to_uint(), (int)comp.s_wb_operand_dbg.read().to_uint()}
    };

    std::cout << IsAHelper::BOLD << "  PIPELINE FLOW:" << IsAHelper::RESET << std::endl;
    for(int i=0; i<4; i++) {
        std::cout << "  " << stages[i].color << std::left << std::setw(12) << stages[i].name << IsAHelper::RESET;
        
        bool is_nop = (stages[i].opcode == 0 && stages[i].operand == 0 && stages[i].pc == 0);
        // Special case for initial state or empty stages
        if (stages[i].pc == 0 && stages[i].opcode == 0 && i > 0) is_nop = true; 

        if (!is_nop) {
            std::string instr_text = IsAHelper::get_instruction_text(stages[i].opcode, stages[i].operand);
            std::cout << " | [PC: " << std::setw(2) << stages[i].pc << "] Instruction: " << IsAHelper::BOLD << std::left << std::setw(18) << instr_text << IsAHelper::RESET;
            if (i == 2) { // EXEC Stage
                 bool is_mem = ((stages[i].opcode & 0xF0) == 0x10 || (stages[i].opcode & 0xF0) == 0x30);
                 if (is_mem) {
                     std::cout << " | MAR: " << std::setw(2) << (int)mar << " | RAM: " << std::setw(3) << (int)ram;
                 } else {
                     std::cout << " | MAR: -- | RAM: ---";
                 }
            }
        } else {
            std::cout << " | [PC: --] Instruction: ---               ";
        }
        std::cout << std::endl;
    }

    // 3. Resultados
    std::string last_res = "Idle";
    for(int i = (int)g_history.size()-1; i>=0; i--) {
        if (!g_history[i].result.empty() && g_history[i].ticks_elapsed >= 3) {
            last_res = g_history[i].result;
            break;
        }
    }
    std::cout << IsAHelper::CYAN << "  " << std::string(75, '-') << IsAHelper::RESET << std::endl;
    std::cout << "  " << IsAHelper::GREEN << "LATEST WB RESULT: " << IsAHelper::BOLD << last_res << IsAHelper::RESET;
    std::cout << "  |  " << IsAHelper::YELLOW << "OUTPUT (OUT): " << IsAHelper::BOLD << g_last_output << IsAHelper::RESET << std::endl;
    std::cout << IsAHelper::CYAN << "  " << std::string(75, '=') << IsAHelper::RESET << std::endl;
}

int sc_main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Uso: " << argv[0] << " <archivo.asm> [--live] [--manual]\n";
        return 1;
    }

    bool live_mode = false;
    bool manual_mode = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--live") live_mode = true;
        else if (arg == "--manual") { live_mode = true; manual_mode = true; }
        else if (arg == "--fast") g_delay_ms = 20;
        else if (arg.find("--delay=") == 0) g_delay_ms = std::stoi(arg.substr(8));
    }

    sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING);
    sc_clock clk("main_clock", 10, SC_NS);
    sc_signal<bool> reset;

    ComputerTop Computer("sap_1_computer");
    Computer.clk(clk);
    Computer.reset(reset);  

    if (!AssemblerLoader::load(argv[1], Computer.DataRam->memory) || !AssemblerLoader::load(argv[1], Computer.InstRom->memory)) {
        return 1;
    }

    reset.write(1);
    sc_start(15, SC_NS);
    reset.write(0);

    bool halted = false;

    // 0. Mostrar estado inicial (tras Reset)
    render_dashboard(Computer, live_mode);
    if (live_mode && manual_mode) {
        std::cout << IsAHelper::YELLOW << "  [MANUAL] Presiona ENTER para iniciar la ejecución..." << std::flush;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Loop de Ejecución Principal
    for(unsigned long cycle = 0; cycle < 2000 && !halted; cycle++) {
        sc_start(10, SC_NS);
        
        // 1. Progresar Pipeline
        for (auto& hist_entry : g_history) {
            hist_entry.ticks_elapsed++;
            
            if (hist_entry.ticks_elapsed == 3 && !hist_entry.logged) {
                hist_entry.logged = true;
                int opc = hist_entry.opcode;
                unsigned char r0_u = Computer.s_rf_r0.read().to_uint();
                unsigned char r1_u = Computer.s_rf_r1.read().to_uint();
                unsigned char r2_u = Computer.s_rf_r2.read().to_uint();
                unsigned char r3_u = Computer.s_rf_r3.read().to_uint();

                if ((opc & 0xF0) == 0x10 || (opc & 0xF0) == 0x20) { // LD/LDI
                    int r_idx = opc & 0x03;
                    signed char val = (r_idx==0)?(signed char)r0_u:(r_idx==1)?(signed char)r1_u:(r_idx==2)?(signed char)r2_u:(signed char)r3_u;
                    hist_entry.result = "R" + std::to_string(r_idx) + " <- " + std::to_string((int)val);
                } else if (opc == OP_ADD_RR || opc == OP_SUB_RR) {
                    int rd = (hist_entry.operand >> 4) & 0x03;
                    hist_entry.result = "R" + std::to_string(rd) + " UPD";
                } else if (opc >= 0xE0 && opc <= 0xE3) { // OUT
                    int r_idx = opc & 0x03;
                    signed char val = (r_idx==0)?(signed char)r0_u:(r_idx==1)?(signed char)r1_u:(r_idx==2)?(signed char)r2_u:(signed char)r3_u;
                    unsigned char uval = (unsigned char)val;
                    char hex_buf[10];
                    sprintf(hex_buf, "0x%X", uval);
                    g_last_output = std::to_string((int)val) + " (Signed) | " + std::to_string((int)uval) + " (Unsig) | " + std::string(hex_buf) + " (Hex)";
                }
            }
        }

        // 2. Logging de nueva instrucción (Solo para el historial de resultados)
        if (Computer.s_pc_en.read() == 1) {
            unsigned int pc = Computer.s_id_pc_dbg.read().to_uint();
            unsigned char opcode = Computer.s_id_opcode.read().to_uint();
            unsigned char operand = Computer.s_id_operand.read().to_uint();

            TraceEntry entry;
            entry.addr = (int)pc;
            entry.full_text = IsAHelper::get_instruction_text(opcode, operand);
            entry.opcode = opcode;
            entry.operand = (int)operand;
            entry.ticks_elapsed = 1; // Ya está en ID
            entry.logged = false;
            g_history.push_back(entry);
            
            if (opcode == OP_HLT) halted = true;
        }

        render_dashboard(Computer, live_mode);
        if (live_mode) {
            if (manual_mode) {
                std::cout << IsAHelper::YELLOW << "  [MANUAL] Presiona ENTER para el siguiente ciclo..." << std::flush;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(g_delay_ms));
            }
        }
        
        if (halted) break;
    }

    // Pipeline Draining tras HLT
    for(int drain=0; drain<4; drain++) {
        sc_start(10, SC_NS);
        for (auto& hist_entry : g_history) {
            hist_entry.ticks_elapsed++;
            if (hist_entry.ticks_elapsed == 3 && !hist_entry.logged) {
                hist_entry.logged = true;
                int opc = hist_entry.opcode;
                unsigned char r0_u = Computer.s_rf_r0.read().to_uint();
                unsigned char r1_u = Computer.s_rf_r1.read().to_uint();
                unsigned char r2_u = Computer.s_rf_r2.read().to_uint();
                unsigned char r3_u = Computer.s_rf_r3.read().to_uint();

                if (opc == OP_ADD_RR || opc == OP_SUB_RR) {
                    int rd = (hist_entry.operand >> 4) & 0x03;
                    hist_entry.result = "R" + std::to_string(rd) + " UPD";
                } else if (opc >= 0xE0 && opc <= 0xE3) {
                    int r_idx = opc & 0x03;
                    signed char val = (r_idx==0)?(signed char)r0_u:(r_idx==1)?(signed char)r1_u:(r_idx==2)?(signed char)r2_u:(signed char)r3_u;
                    unsigned char uval = (unsigned char)val;
                    char hex_buf[10];
                    sprintf(hex_buf, "0x%X", uval);
                    g_last_output = std::to_string((int)val) + " (Signed) | " + std::to_string((int)uval) + " (Unsig) | " + std::string(hex_buf) + " (Hex)";
                }
            }
        }
        render_dashboard(Computer, live_mode);
        if (live_mode) {
            if (manual_mode) {
                std::cout << IsAHelper::YELLOW << "  [MANUAL] Presiona ENTER para el siguiente ciclo..." << std::flush;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(g_delay_ms));
            }
        }
    }

    if (live_mode) {
        std::cout << IsAHelper::RED << IsAHelper::BOLD << "\n SIMULACIÓN FINALIZADA." << IsAHelper::RESET << std::endl;
        std::cout << IsAHelper::BOLD << "Tiempo total de ejecución: " << sc_time_stamp() << IsAHelper::RESET << std::endl;
    }

    return 0;
}
