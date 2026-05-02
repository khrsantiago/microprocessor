#ifndef ACCUMULATOR_H
#define ACCUMULATOR_H

#include <systemc.h>

// Registro de 4 bits con señal de Carga (Load Enable)
SC_MODULE(Accumulator) {
    sc_in<bool> clk;              // Reloj del sistema
    sc_in<bool> ld;               // Load Enable (Habilitador de carga)
    sc_in<sc_uint<4>> d;          // Entrada de datos desde la ALU
    sc_out<sc_uint<4>> q;         // Salida del acumulador (Estado)

    // Variable interna para mantener el estado actual
    sc_uint<4> current_value;

    void update_logic() {
        // Solo actuamos en el flanco de subida del reloj
        if (clk.posedge()) {
            // Verificamos el "portero": ¿Está habilitada la carga?
            if (ld.read() == true) {
                current_value = d.read(); // Guardamos el nuevo valor
            }
            // Si ld es 0, no hacemos nada, lo que mantiene el valor anterior
            q.write(current_value);
        }
    }

    SC_CTOR(Accumulator) {
        SC_METHOD(update_logic);
        sensitive << clk.pos(); // Sensible al flanco de subida
        
        current_value = 0; // Inicializamos el acumulador en 0
    }
};

#endif