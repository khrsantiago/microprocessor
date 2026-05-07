#include <systemc.h>
#include <iomanip>
#include "computer_top.h"
#include "assembler.h"
#include "isa_helper.h"

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
    if (!AssemblerLoader::load(argv[1], Computer.DataRam->memory) || !AssemblerLoader::load(argv[1], Computer.InstRom->memory)) {
        return 1;
    }

    std::cout << "\n--- Iniciando Simulacion de KHR-16 ---\n";
    std::cout << "Archivo: " << argv[1] << "\n";
    std::cout << "--------------------------------------------------------------------------\n";

    // Pulso de Reset inicial
    reset.write(1);
    sc_start(15, SC_NS);
    reset.write(0);

    // Ejecutar la simulación
    // Le damos un máximo de 200 ciclos o hasta que encuentre un HLT
    bool halted = false;
    for(int i = 0; i < 5000 && !halted; i++) {
        unsigned int pc_if = Computer.s_if_pc_dbg.read().to_uint();
        unsigned int pc_id = Computer.s_id_pc_dbg.read().to_uint();
        unsigned int pc_ex = Computer.s_ex_pc_dbg.read().to_uint();
        unsigned int pc_wb = Computer.s_wb_pc_dbg.read().to_uint();

        unsigned int op_id = Computer.s_ir_opcode.read().to_uint();
        unsigned int opr_id = Computer.s_ir_operand.read().to_uint();
        std::cout << IsAHelper::CYAN << "[ " << std::setw(5) << sc_time_stamp() << " ] (Estable)" << IsAHelper::RESET << "\n";
        
        uint16_t if_op = Computer.s_if_opcode_dbg.read().to_uint();
        uint16_t if_opr = Computer.s_if_operand_dbg.read().to_uint();
        std::cout << "  FETCH  (IF) | [PC: " << std::setw(3) << pc_if << "] Instruction: ";
        if (if_op == OP_ADD_RR || if_op == OP_SUB_RR) {
            std::cout << IsAHelper::get_mnemonic(if_op) << " R" << ((if_opr >> 4) & 0x3) << ", R" << (if_opr & 0x3) << "\n";
        } else {
            std::cout << IsAHelper::get_mnemonic(if_op) << " " << (int)if_opr << "\n";
        }
        
        std::cout << "  DECODE (ID) | [PC: " << std::setw(3) << pc_id << "] Instruction: ";
        if (op_id == OP_ADD_RR || op_id == OP_SUB_RR) {
            std::cout << IsAHelper::get_mnemonic(op_id) << " R" << ((opr_id >> 4) & 0x3) << ", R" << (opr_id & 0x3) << "\n";
        } else {
            std::cout << IsAHelper::get_mnemonic(op_id) << " " << (int)opr_id << "\n";
        }
        
        std::cout << "  EXEC   (EX) | [PC: " << std::setw(3) << pc_ex << "] Instruction: ";
        uint16_t ex_op = Computer.s_ex_opcode_dbg.read().to_uint();
        uint16_t ex_opr = Computer.s_ex_operand_dbg.read().to_uint();
        if (ex_op == OP_ADD_RR || ex_op == OP_SUB_RR) {
             std::cout << IsAHelper::get_mnemonic(ex_op) << " R" << ((ex_opr >> 4) & 0x3) << ", R" << (ex_opr & 0x3) << "   ";
        } else {
             std::cout << std::left << std::setw(6) << IsAHelper::get_mnemonic(ex_op) << " " << std::left << std::setw(5) << ex_opr;
        }

        // Solo mostrar MAR/RAM si es una instrucción de memoria
        bool ex_is_mem = ((ex_op & 0xF0) == 0x10 || (ex_op & 0xF0) == 0x30 || (ex_op & 0xF0) == 0x40 || (ex_op & 0xF0) == 0x50);
        if (ex_is_mem) {
            std::cout << " | MAR: " << std::right << std::setw(4) << (int)Computer.s_ex_ram_addr.read().to_uint() 
                      << " | RAM: " << std::setw(5) << (int)Computer.s_ram_to_bus.read().to_uint() << "\n";
        } else {
            std::cout << " | MAR: -- | RAM: ---" << "\n";
        }
        
        std::cout << "  WBACK  (WB) | [PC: " << std::setw(3) << pc_wb << "] Instruction: ";
        uint16_t wb_op = Computer.s_wb_opcode_dbg.read().to_uint();
        uint16_t wb_opr = Computer.s_wb_operand_dbg.read().to_uint();
        if (wb_op == OP_ADD_RR || wb_op == OP_SUB_RR) {
             std::cout << IsAHelper::get_mnemonic(wb_op) << " R" << ((wb_opr >> 4) & 0x3) << ", R" << (wb_opr & 0x3) << "   ";
        } else {
             std::cout << std::left << std::setw(6) << IsAHelper::get_mnemonic(wb_op) << " " << std::left << std::setw(5) << wb_opr;
        }
        std::cout << " | R0:" << std::setw(6) << (int)Computer.s_rf_r0.read().to_uint() 
                  << " R1:" << std::setw(6) << (int)Computer.s_rf_r1.read().to_uint()
                  << " R2:" << std::setw(6) << (int)Computer.s_rf_r2.read().to_uint()
                  << " R3:" << std::setw(6) << (int)Computer.s_rf_r3.read().to_uint()
                  << " | Bus: 0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << (int)Computer.common_bus.read().to_uint() << std::dec << std::setfill(' ') << "\n"
                  << "                | Flags: Z:" << Computer.s_flags_z.read() << " C:" << Computer.s_flags_c.read() << " N:" << Computer.s_flags_n.read() << " V:" << Computer.s_flags_o.read()
                  << "\n";

        std::cout << "  --------------------------------------------------------------------------\n";

        // Avanzar el reloj para el siguiente ciclo
        sc_start(10, SC_NS);

        // Si el OpCode es HLT (0xFF) en la etapa de DECODE
        if (Computer.s_ir_opcode.read().to_uint() == OP_HLT) {
            halted = true;
            std::cout << "\n[HALT] Instrucción HLT detectada. Finalizando últimos ciclos:\n";
            
            auto print_map = [&]() {
                unsigned int pc_if = Computer.s_if_pc_dbg.read().to_uint();
                unsigned int pc_id = Computer.s_id_pc_dbg.read().to_uint();
                unsigned int pc_ex = Computer.s_ex_pc_dbg.read().to_uint();
                unsigned int pc_wb = Computer.s_wb_pc_dbg.read().to_uint();

                std::cout << IsAHelper::YELLOW << "[ " << std::setw(5) << sc_time_stamp() << " ]" 
                          << " (Drenando...)" << IsAHelper::RESET << "\n";
                
                std::cout << "  FETCH  (IF) | [PC: " << std::setw(2) << pc_if << "] Instruction: "
                          << IsAHelper::get_mnemonic(Computer.s_if_opcode_dbg.read().to_uint()) << " " 
                          << (int)Computer.s_if_operand_dbg.read().to_uint() << "\n";
                
                std::cout << "  DECODE (ID) | [PC: " << std::setw(2) << pc_id << "] Instruction: " 
                          << IsAHelper::get_mnemonic(Computer.s_ir_opcode.read().to_uint()) << " " 
                          << (int)Computer.s_ir_operand.read().to_uint() << "\n";
                
                std::cout << "  EXEC   (EX) | [PC: " << std::setw(2) << pc_ex << "] Instruction: ";
                unsigned int ex_op = Computer.s_ex_opcode_dbg.read().to_uint();
                unsigned int ex_opr = Computer.s_ex_operand_dbg.read().to_uint();
        if (ex_op == OP_ADD_RR || ex_op == OP_SUB_RR) {
             std::cout << IsAHelper::get_mnemonic(ex_op) << " R" << ((ex_opr >> 4) & 0x3) << ", R" << (ex_opr & 0x3) << "   ";
        } else {
             std::cout << std::left << std::setw(6) << IsAHelper::get_mnemonic(ex_op) << " " << std::left << std::setw(3) << ex_opr;
        }

        // Solo mostrar MAR/RAM si es una instrucción de memoria (LD=0x10, ST=0x30)
        bool is_mem = ((ex_op & 0xF0) == 0x10 || (ex_op & 0xF0) == 0x30);
        if (is_mem) {
            std::cout << " | MAR: " << std::right << std::setw(2) << (int)Computer.s_mar_to_ram.read().to_uint() 
                      << " | RAM: " << std::setw(3) << (int)Computer.s_ram_to_bus.read().to_uint() << "\n";
        } else {
            std::cout << " | MAR: -- | RAM: ---" << "\n";
        }
                
                std::cout << "  WBACK  (WB) | [PC: " << std::setw(2) << pc_wb << "] Instruction: ";
                unsigned int wb_op = Computer.s_wb_opcode_dbg.read().to_uint();
                unsigned int wb_opr = Computer.s_wb_operand_dbg.read().to_uint();
                if (wb_op == OP_ADD_RR || wb_op == OP_SUB_RR) {
                     std::cout << IsAHelper::get_mnemonic(wb_op) << " R" << ((wb_opr >> 4) & 0x3) << ", R" << (wb_opr & 0x3) << "   ";
                } else {
                     std::cout << std::left << std::setw(6) << IsAHelper::get_mnemonic(wb_op) << " " << std::left << std::setw(3) << wb_opr;
                }
                std::cout << " | R0:" << std::setw(2) << (int)Computer.s_rf_r0.read().to_uint() 
                          << " R1:" << std::setw(2) << (int)Computer.s_rf_r1.read().to_uint()
                          << " R2:" << std::setw(2) << (int)Computer.s_rf_r2.read().to_uint()
                          << " R3:" << std::setw(2) << (int)Computer.s_rf_r3.read().to_uint()
                          << " | Bus: " << std::setw(3) << (int)Computer.common_bus.read().to_uint() << "\n"
                          << "              | Flags: Z:" << Computer.s_flags_z.read() << " C:" << Computer.s_flags_c.read() << " N:" << Computer.s_flags_n.read() << " V:" << Computer.s_flags_o.read()
                          << "\n";

                std::cout << "  --------------------------------------------------------------------------\n";
            };

            for(int drain = 0; drain < 4; drain++) {
                print_map();
                sc_start(10, SC_NS);
            }
            std::cout << "\n[HALT] Procesamiento detenido por instrucción HLT.\n";
        }
    }

    if (!halted) {
        std::cout << "\n[TIMEOUT] La simulación terminó por límite de ciclos.\n";
    }

    return 0;
}
