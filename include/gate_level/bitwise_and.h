#ifndef BITWISE_AND_H
#define BITWISE_AND_H

#include <systemc.h>
#include "and_gate.h"

// Modulo estructural para AND de 8 bits
SC_MODULE(BitwiseAnd) {
    // Puertos externos (Buses de 8 bits)
    sc_in<sc_uint<8>> a;
    sc_in<sc_uint<8>> b;
    sc_out<sc_uint<8>> result;

    // Instancias de los 8 chips de 1 bit
    AndGate *gates[8];

    // Cables internos para separar y unir los buses
    sc_signal<bool> sig_a[8], sig_b[8], sig_res[8];

    // Proceso para desempaquetar los buses en cables individuales
    void split_buses() {
        for(int i = 0; i < 8; i++) {
            sig_a[i].write(a.read()[i]);
            sig_b[i].write(b.read()[i]);
        }
    }

    // Proceso para empaquetar los cables individuales en el bus de salida
    void merge_buses() {
        sc_uint<8> temp;
        for(int i = 0; i < 8; i++) {
            temp[i] = sig_res[i].read();
        }
        result.write(temp);
    }

    // Constructor (La placa de circuito impreso)
    SC_CTOR(BitwiseAnd) {
        for(int i = 0; i < 8; i++) {
            std::string name = "gate" + std::to_string(i);
            gates[i] = new AndGate(name.c_str());
            gates[i]->a(sig_a[i]);
            gates[i]->b(sig_b[i]);
            gates[i]->y(sig_res[i]);
        }

        // Registramos los procesos de cableado
        SC_METHOD(split_buses);
        sensitive << a << b;

        SC_METHOD(merge_buses);
        for(int i = 0; i < 8; i++) sensitive << sig_res[i];
    }

    // Destructor para limpiar la memoria
    ~BitwiseAnd() {
        for(int i = 0; i < 8; i++) delete gates[i];
    }
};

#endif // BITWISE_AND_H