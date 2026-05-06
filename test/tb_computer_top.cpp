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

    // --- PROGRAMA DE PRUEBA GPR ---
    // 0: LDI R0, 10
    Computer.InstRom->memory[0] = OP_LDI_R0;
    Computer.InstRom->memory[1] = 10;
    
    // 2: OUT R0
    Computer.InstRom->memory[2] = OP_OUT_R0;
    Computer.InstRom->memory[3] = 0;
    
    // 4: JMP 0
    Computer.InstRom->memory[4] = OP_JMP;
    Computer.InstRom->memory[5] = 0;
    
    std::cout << "Starting 'GPR Handshake' Test (LDI R0, 10)...\n";
    std::cout << "Time  | PC | IR (Op|Opr) | R0 | R1 | Bus  | Status\n";
    std::cout << "---------------------------------------------------\n";

    // Reset
    reset.write(1);
    sc_start(15, SC_NS);
    reset.write(0);

    for(int i = 0; i < 15; i++) {
        sc_start(10, SC_NS);
        std::cout << std::setw(5) << sc_time_stamp() << " | "
                  << std::setw(2) << Computer.s_pc_to_bus.read().to_uint() << " |    "
                  << std::hex << Computer.s_ir_opcode.read().to_uint() << "|" 
                  << std::hex << Computer.s_ir_operand.read().to_uint() << std::dec << "    | "
                  << std::setw(2) << Computer.s_rf_r0.read().to_uint() << " | "
                  << std::setw(2) << Computer.s_rf_r1.read().to_uint() << " | "
                  << std::setw(3) << Computer.common_bus.read().to_uint() << " | RUN\n";
    }

    // --- PROGRAMA DE CONTEO (Hello GPR) ---
    // 0: LDI R0, 0
    Computer.InstRom->memory[0] = OP_LDI_R0; Computer.InstRom->memory[1] = 0;
    // 2: LDI R1, 1
    Computer.InstRom->memory[2] = OP_LDI_R1; Computer.InstRom->memory[3] = 1;
    // 4: ADD R0, R1 (Operand 0x01: Rd=0, Rs=1)
    Computer.InstRom->memory[4] = OP_ADD_RR; Computer.InstRom->memory[5] = 0x01;
    // 6: OUT R0
    Computer.InstRom->memory[6] = OP_OUT_R0; Computer.InstRom->memory[7] = 0;
    // 8: JMP 4
    Computer.InstRom->memory[8] = OP_JMP; Computer.InstRom->memory[9] = 4;

    reset.write(1);
    sc_start(15, SC_NS);
    reset.write(0);

    std::cout << "\nStarting 'Hello GPR' Counting Loop...\n";
    std::cout << "Time  | PC | IR (Op|Opr) | R0 | R1 | Bus  | Status\n";
    std::cout << "---------------------------------------------------\n";

    for(int i = 0; i < 40; i++) { 
        sc_start(10, SC_NS);
        std::cout << std::setw(5) << sc_time_stamp() << " | "
                  << std::setw(2) << Computer.s_pc_to_bus.read().to_uint() << " |    "
                  << std::hex << Computer.s_ir_opcode.read().to_uint() << "|" 
                  << std::hex << Computer.s_ir_operand.read().to_uint() << std::dec << "    | "
                  << std::setw(2) << Computer.s_rf_r0.read().to_uint() << " | "
                  << std::setw(2) << Computer.s_rf_r1.read().to_uint() << " | "
                  << std::setw(3) << Computer.common_bus.read().to_uint() << " | PIPE\n";
    }

    return 0;
}