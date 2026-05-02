#ifndef REGISTER_4BIT_H
#define REGISTER_4BIT_H

#include <systemc.h>

// Modulo de Registro de 4 bits basado en Flip-Flops tipo D
SC_MODULE(Register4Bit) {
    // Puertos de hardware
    sc_in<bool> clk;              // Reloj del sistema
    sc_in<sc_uint<4>> d;          // Entrada de datos
    sc_out<sc_uint<4>> q;         // Salida de datos / Estado

    // Logica secuencial: Se ejecuta en los latidos del reloj
    void update_state() {
        // Copiamos la entrada a la salida.
        // Debido a la lista de sensibilidad del constructor,
        // esto SOLO ocurrira cuando el reloj suba.
        q.write(d.read());
    }

    // Constructor de la placa
    SC_CTOR(Register4Bit) {
        SC_METHOD(update_state);
        // ¡LA MAGIA OCURRE AQUI! 
        // Solo somos sensibles al flanco de subida (pos = positive) de la señal 'clk'
        sensitive << clk.pos(); 
    }
};

#endif // REGISTER_4BIT_H