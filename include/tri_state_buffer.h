#ifndef TRI_STATE_BUFFER_H
#define TRI_STATE_BUFFER_H

#include <systemc.h>

// Buffer Tri-state de 8 bits para conexion al Bus Comun
SC_MODULE(TriStateBuffer) {
    sc_in<sc_uint<8>> data_in;  // Entrada de datos del modulo (ALU, RAM, etc.)
    sc_in<bool> enable;         // Pin de control: 1 = Habla, 0 = Silencio (Hi-Z)
    // CAMBIO CRITICO: 'sc_out_rv' permite multiples conductores de forma segura
    sc_out_rv<8> bus_out;

    void process() {
        if (enable.read() == true) {
            // Convertimos el valor numerico a vector logico y lo enviamos al bus
            bus_out.write(sc_lv<8>(data_in.read()));
        } else {
            // Ponemos la salida en Alta Impedancia (Alta Z)
            // Esto "desconecta" el modulo del bus central
            bus_out.write("ZZZZZZZZ");
        }
    }

    SC_CTOR(TriStateBuffer) {
        SC_METHOD(process);
        sensitive << data_in << enable;
    }
};

#endif