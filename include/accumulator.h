#ifndef ACCUMULATOR_H
#define ACCUMULATOR_H

#include <systemc.h>

// Registro de 8 bits con señal de Carga (Load Enable)
SC_MODULE(Accumulator) {
    sc_in<bool> clk;              // Reloj del sistema
    sc_in<bool> ld;               // Load Enable (Habilitador de carga)
    sc_in<sc_uint<8>> d;          // Entrada de datos desde la ALU (y bus)
    sc_out<sc_uint<8>> q;         // Salida del acumulador (Estado)
    sc_out<bool> flag_z;          // Bandera de Cero (Zero Flag latched)
    sc_in<bool> carry_in;         // Acarreo desde la ALU
    sc_out<bool> flag_c;          // Bandera de Acarreo (Carry Flag latched)

    // Variable interna para mantener el estado actual
    sc_uint<8> current_value;
    bool current_carry;

    void update_logic() {
        // Solo actuamos en el flanco de subida del reloj
        if (clk.posedge()) {
            // Verificamos el "portero": ¿Está habilitada la carga?
            if (ld.read() == true) {
                current_value = d.read(); // Guardamos el nuevo valor
                current_carry = carry_in.read(); // Guardamos el acarreo
            }
            // Si ld es 0, no hacemos nada, lo que mantiene el valor anterior
            q.write(current_value);
            flag_z.write(current_value == 0);
            flag_c.write(current_carry);
        }
    }

    SC_CTOR(Accumulator) {
        SC_METHOD(update_logic);
        sensitive << clk.pos(); // Sensible al flanco de subida
        
        current_value = 0; // Inicializamos el acumulador en 0
        current_carry = false;
        flag_z.initialize(true); // En 0, el flag Z es verdadero
        flag_c.initialize(false);
    }
};

#endif