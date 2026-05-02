#ifndef COMPUTER_TOP_H
#define COMPUTER_TOP_H

#include <systemc.h>

// Incluimos todos los modulos construidos anteriormente
#include "program_counter.h"
#include "mar.h"
#include "ram.h"
#include "instruction_register.h"
#include "accumulator.h"
#include "alu.h"
#include "control_unit.h"
#include "tri_state_buffer.h"
#include "output_register.h"

SC_MODULE(ComputerTop) {
    // Puertos externos (Reloj y Reset maestro)
    sc_in<bool> clk;
    sc_in<bool> reset;

    // --- EL SISTEMA NERVIOSO (Senales Internas) ---

    // 1. El Bus Central (Resolved para evitar conflictos)
    sc_signal_rv<4> common_bus;

    // 2. Senales de Control (Emitidas por la CU)
    sc_signal<bool> s_pc_out, s_pc_inc, s_mar_in, s_ram_out, s_ir_in, s_ir_out, s_a_in;
    sc_signal<bool> s_pc_load;

    sc_signal<bool> s_ram_we; // Tierra (siempre 0 para lectura)
    sc_signal<bool> s_alu_zero_flag;

    sc_signal<bool> s_a_out;
    sc_signal<bool> s_out_load;

    sc_signal<sc_uint<4>> s_display_val;

    sc_signal<bool> s_b_in;    // Para atrapar datos del bus al Registro B
    sc_signal<bool> s_alu_out; // Para enviar el resultado de la ALU al Bus
    sc_signal<sc_uint<2>> s_alu_op; // 0 = Suma, 1 = Resta

    // 3. Cables de Datos Especificos
    sc_signal<sc_uint<4>> s_pc_to_bus;   // Salida del PC antes del Buffer
    sc_signal<sc_uint<4>> s_ram_to_bus;  // Salida de la RAM antes del Buffer
    sc_signal<sc_uint<4>> s_mar_to_ram; // Salida limpia del MAR
    sc_signal<sc_lv<4>> s_ir_opcode;     // OpCode desde el IR a la CU
    sc_signal<sc_lv<4>> s_ir_operand;    // Operando desde el IR al Bus
    //sc_signal<sc_uint<4>> s_mar_addr;    // Direccion desde el MAR a la RAM
    sc_signal<sc_uint<4>> s_rega_out;    // Salida del acumulador

    sc_signal<sc_uint<4>> s_breg_out;   // Del RegB directo a la ALU
    sc_signal<sc_uint<4>> s_alu_result; // De la ALU a su Buffer

    // Adaptadores de Pegamento
    sc_signal<sc_uint<4>> s_bus_to_uint;
    sc_signal<sc_lv<8>>   s_bus_to_ir_8;
    //sc_signal<sc_uint<4>> s_bus_to_ram_uint; // Convierte el Bus a numero para la RAM
    //sc_signal<sc_lv<8>>   s_bus_to_ir_lv8;   // Expande el Bus de 4 a 8 bits para el IR
    sc_signal<sc_uint<4>> s_opcode_to_cu;    // Convierte el OpCode a numero para la CU
    sc_signal<sc_uint<4>> s_operand_to_pc;

    //sc_signal<bool> s_ram_we_read_only;    

    // --- INSTANCIAS DE LOS COMPONENTES ---
    ProgramCounter *Pc;
    InstructionRegister *Ir;
    MemoryAddressRegister *Mar;
    RAM *Ram;
    Accumulator *RegA;
    Accumulator *RegB;
    ALU *Alu;
    OutputRegister *OutReg;
    ControlUnit *Cu;

    TriStateBuffer *BufPc;
    TriStateBuffer *BufRam;
    TriStateBuffer *BufIr;
    TriStateBuffer *BufRegA;
    TriStateBuffer *BufAlu;

    // --- PROCESO TRADUCTOR (GLUE LOGIC) ---
    // Glue Logic: Traductor de tipos y tamaños
    void signal_adapters() {
        sc_lv<4> bus_val = common_bus.read();
        
        // 1. Bus -> RAM
        if (bus_val.is_01()) s_bus_to_uint.write(bus_val.to_uint());
        
        // 2. Bus -> IR (El Hack de Integración)
        sc_lv<8> exp = "00000000";
        
        // Si la Unidad de Control enciende la señal de "Atrapar Instruccion" (T3)
        if (s_ir_in.read() == 1) {
            // Leemos el byte (8 bits) directamente de la RAM usando la direccion del MAR
            int addr = s_mar_to_ram.read().to_uint();
            int full_instruction = Ram->memory[addr];
            
            // Asignamos el valor real inyectado por el ensamblador
            exp = full_instruction; 
        } else {
            // Comportamiento normal: el bus entra en los 4 bits inferiores
            exp.range(7, 4) = "0000";
            exp.range(3, 0) = bus_val;
        }
        s_bus_to_ir_8.write(exp);

        // 3. IR -> CU y PC
        if (s_ir_opcode.read().is_01()) s_opcode_to_cu.write(s_ir_opcode.read().to_uint());
        if (s_ir_operand.read().is_01()) s_operand_to_pc.write(s_ir_operand.read().to_uint());
    }

    SC_CTOR(ComputerTop) {

        // Registramos el traductor de senales para que corra continuamente
        SC_METHOD(signal_adapters);
        sensitive << common_bus << s_ir_opcode << s_ir_operand;

        // 1. Program Counter
        Pc = new ProgramCounter("PC");
        Pc->clk(clk); Pc->reset(reset);
        Pc->en(s_pc_inc); Pc->load(s_pc_load);
        Pc->q(s_pc_to_bus);
        Pc->data_in(s_operand_to_pc);

        // 2. Buffer PC -> Bus
        BufPc = new TriStateBuffer("BUF_PC");
        BufPc->data_in(s_pc_to_bus);
        BufPc->enable(s_pc_out);
        BufPc->bus_out(common_bus);

        // MAR
        Mar = new MemoryAddressRegister("MAR");
        Mar->clk(clk); Mar->ld(s_mar_in);
        Mar->data_in(common_bus); // Escucha directamente el bus
        Mar->addr_out(s_mar_to_ram); // Entrega la direccion a la RAM

        // 3. RAM
        Ram = new RAM("RAM");
        Ram->clk(clk);
        Ram->we(s_ram_we); // Memoria estrictamente protegida
        Ram->addr(s_mar_to_ram); // Conectado a la salida del MAR/IR
        Ram->data_in(s_bus_to_uint); // Para futuras escrituras
        Ram->data_out(s_ram_to_bus);

        // 4. Buffer RAM -> Bus
        BufRam = new TriStateBuffer("BUF_RAM");
        BufRam->data_in(s_ram_to_bus);
        BufRam->enable(s_ram_out);
        BufRam->bus_out(common_bus);

        // 5. Instruction Register
        Ir = new InstructionRegister("IR");
        Ir->clk(clk); Ir->ld(s_ir_in);
        Ir->data_in(s_bus_to_ir_8); // Captura desde el Bus
        Ir->opcode_out(s_ir_opcode);
        Ir->operand_out(s_ir_operand);

        // 8. Buffer IR -> Bus (Para que el operando llegue al MAR en T4)
        BufIr = new TriStateBuffer("BUF_IR");
        BufIr->data_in(s_operand_to_pc); // Reusamos este cable que ya traduce a numero puro
        BufIr->enable(s_ir_out);
        BufIr->bus_out(common_bus);

        // 6. Unidad de Control (El Cerebelo)
        Cu = new ControlUnit("CU");
        Cu->clk(clk); Cu->reset(reset);
        Cu->opcode(s_opcode_to_cu); // Lee el OpCode del IR
        Cu->pc_out(s_pc_out); Cu->pc_inc(s_pc_inc); Cu->pc_load(s_pc_load);
        Cu->mar_in(s_mar_in); Cu->ram_out(s_ram_out);
        Cu->ir_in(s_ir_in);   Cu->ir_out(s_ir_out);
        Cu->a_in(s_a_in); Cu->flag_z(s_alu_zero_flag);
        Cu->out_load(s_out_load);
        Cu->a_out(s_a_out);
        Cu->b_in(s_b_in);
        Cu->alu_out(s_alu_out);
        Cu->alu_op(s_alu_op);

        // 7. Acumulador (Registro A)
        RegA = new Accumulator("RegA");
        RegA->clk(clk);
        RegA->ld(s_a_in);
        // Reusamos el adaptador del bus a uint porque el Acumulador 
        // tambien necesita leer un numero limpio
        RegA->d(s_bus_to_uint); 
        RegA->q(s_rega_out); 

        // 8. Registro B (Sostiene el segundo operando)
        RegB = new Accumulator("RegB"); // Usamos la misma estructura de registro
        RegB->clk(clk);
        RegB->ld(s_b_in);
        RegB->d(s_bus_to_uint); // Escucha al bus
        RegB->q(s_breg_out);    // Su salida va DIRECTO a la ALU, no al bus

        // 9. La Unidad Aritmetico Logica (Tu diseño avanzado)
        Alu = new ALU("ALU");
        Alu->a(s_rega_out); 
        Alu->b(s_breg_out); 
        Alu->op(s_alu_op);           // Usamos tu selector de 2 bits
        Alu->result(s_alu_result); 
        Alu->zero_flag(s_alu_zero_flag); // Usamos tu nombre exacto para el pin

        // 10. Buffer ALU -> Bus
        BufAlu = new TriStateBuffer("BUF_ALU");
        BufAlu->data_in(s_alu_result);
        BufAlu->enable(s_alu_out);
        BufAlu->bus_out(common_bus);

        // El Buffer que conecta la salida de RegA con el Bus
        BufRegA = new TriStateBuffer("BUF_REGA");
        BufRegA->data_in(s_rega_out); // Escucha al RegA
        BufRegA->enable(s_a_out);     // Controlado por la CU (a_out)
        BufRegA->bus_out(common_bus); // Habla con el Bus Central

        // 3. Connect the Output Register
        OutReg = new OutputRegister("OUT_REG");
        OutReg->clk(clk);
        OutReg->load(s_out_load); // Connected to Control Unit
        OutReg->data_in(common_bus); // Listens to the Bus
        OutReg->display(s_display_val);


        s_ram_we.write(0);
    }

    ~ComputerTop() {
        if(Pc) delete Pc; 
        if(Ram) delete Ram;
        if(Mar) delete Mar; 
        if(Ir) delete Ir; 
        if(RegA) delete RegA; 
        if(RegB) delete RegB;
        if(Alu) delete Alu; 
        if(OutReg) delete OutReg;
        if(Cu) delete Cu;

        if(BufPc) delete BufPc;
        if(BufRam) delete BufRam;
        if(BufIr) delete BufIr;
        if(BufRegA) delete BufRegA;
        if(BufAlu) delete BufAlu;
    }
};

#endif