#ifndef CONTROL_UNIT_H
#define CONTROL_UNIT_H

#include <systemc.h>
#include "isa.h"

// Definicion de los estados de tiempo para el ciclo Fetch
enum CycleState { T1, T2, T3, T4, T5, T6 };

SC_MODULE(ControlUnit) {
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<sc_uint<4>> opcode; // Entrada de los 4 bits superiores del IR
    sc_in<bool> flag_z;

    // Senales de control de salida (El panel de interruptores de la maquina)
    sc_out<bool> pc_out;   // Habilita el buffer Tri-state del PC hacia el Bus
    sc_out<bool> mar_in;   // Abre la compuerta de carga del MAR
    sc_out<bool> pc_inc;   // Ordena al PC sumar +1
    sc_out<bool> ram_out;  // Habilita la salida de la RAM hacia el Bus
    sc_out<bool> ir_in;    // Abre la compuerta de carga del IR    
    sc_out<bool> ir_out; // Habilita la salida del Operando del IR al Bus
    sc_out<bool> a_in;   // Habilita la carga (Load) en el Acumulador (Registro A)
    sc_out<bool> pc_load; // Cable para disparar el salto

    sc_out<bool> a_out;    // To push Accumulator to the Bus
    sc_out<bool> out_load; // To latch data into the Output Register

    sc_out<bool> b_in, alu_out;
    sc_out<sc_uint<2>> alu_op;

    // Variable interna para almacenar el estado actual
    sc_signal<CycleState> current_state;

    // Proceso 1: Transicion de Estados (Secuencial)
    void state_transition() {
        if (reset.read() == 1) {
            current_state.write(T1);
        } else {
            // Avanzamos al siguiente estado en cada flanco de subida
            switch (current_state.read()) {
                case T1: current_state.write(T2); break;
                case T2: current_state.write(T3); break;
                case T3: current_state.write(T4); break;
                case T4: current_state.write(T5); break;
                case T5: current_state.write(T6); break;
                case T6: current_state.write(T1); break; // Nuevo Fetch
                default: current_state.write(T1); break;
            }
        }
    }

    // Proceso 2: Logica de Salidas (Combinacional)
    void output_logic() {
        // Por seguridad, apagamos TODAS las senales al inicio de la evaluacion
        pc_out.write(0); mar_in.write(0); pc_inc.write(0);
        ram_out.write(0); ir_in.write(0); ir_out.write(0); a_in.write(0);
        pc_load.write(0);
        a_out.write(0); out_load.write(0);
        b_in.write(0); alu_out.write(0);
        alu_op.write(0);

        sc_uint<4> op = opcode.read();

        // Encendemos solo las senales pertinentes al estado actual
        switch (current_state.read()) {
            // --- FASE DE FETCH (Comun para todas las instrucciones) ---
            case T1: pc_out.write(1); mar_in.write(1); break;
            case T2: pc_inc.write(1); break;
            case T3: ram_out.write(1); ir_in.write(1); break;
            
            // --- FASE DE EXECUTE (Depende del OpCode) ---
            case T4:
                if (op == OP_LDA) { // Si OpCode == LDA (0001)
                    ir_out.write(1); // El IR pone la direccion (Operando) en el bus
                    mar_in.write(1); // El MAR atrapa esa direccion
                }
                else if (op == OP_JMP) { // JMP (1000)
                    ir_out.write(1);   // El IR pone la direccion de destino en el bus
                    pc_load.write(1);  // El PC atrapa ese destino y se sobrescribe
                }
                else if (op == OP_OUT) {
                    // Transfer from RegA to OUT_Reg
                    a_out.write(1);
                    out_load.write(1);
                }
                else if (op == OP_JZ) {
                    ir_out.write(1); // Siempre ponemos la direccion destino en el bus
                    
                    // AQUI NACE EL CRITERIO:
                    if (flag_z.read() == true) {
                        // Si el resultado anterior fue Cero, SOBRESCRIBIMOS el PC
                        pc_load.write(1); 
                    } else {
                        // Si no fue Cero, el PC mantiene su valor (que ya fue 
                        // incrementado en T2). No activamos pc_load.
                        // La ejecucion "caera" a la siguiente instruccion.
                        pc_load.write(0); 
                    }
                }
                else if (op == OP_ADD) {
                    ir_out.write(1); // El IR pone la direccion de la variable en el bus
                    mar_in.write(1); // El MAR la atrapa
                }
                break;
            case T5:
                if (op == OP_LDA) { // Si OpCode == LDA (0001)
                    ram_out.write(1); // La RAM pone el dato en el bus
                    a_in.write(1);    // El Acumulador atrapa el dato
                }
                else if (op == OP_ADD) {
                    ram_out.write(1); // La RAM escupe la variable (ej. el '1') al bus
                    b_in.write(1);    // El Registro B la atrapa. (La ALU suma inmediatamente)
                }
                break;
            case T6:
                // T6 se usa como ciclo inactivo (NOP) para estabilizacion 
                // en la instruccion LDA, o para calculos mas complejos en otras.
                if (op == OP_ADD) {
                    alu_out.write(1); // Abrimos el buffer de la ALU, inundando el bus con el resultado
                    a_in.write(1);    // El Acumulador atrapa el nuevo resultado, sobrescribiendose a si mismo
                }
                break;
                
            default: break;
        }
    }

    SC_CTOR(ControlUnit) {
        SC_METHOD(state_transition);
        sensitive << clk.pos() << reset;

        SC_METHOD(output_logic);
        // Sensible a cualquier cambio en el estado interno y a cualquier cambio repentino en el OpCode
        sensitive << current_state << opcode << flag_z;
    }
};

#endif // CONTROL_UNIT_H