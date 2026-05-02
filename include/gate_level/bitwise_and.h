#ifndef BITWISE_AND_H
#define BITWISE_AND_H

#include <systemc.h>
#include "and_gate.h"

// Modulo estructural para AND de 4 bits
SC_MODULE(BitwiseAnd) {
    // Puertos externos (Buses de 4 bits)
    sc_in<sc_uint<4>> a;
    sc_in<sc_uint<4>> b;
    sc_out<sc_uint<4>> result;

    // Instancias de los 4 chips de 1 bit
    AndGate *gate0, *gate1, *gate2, *gate3;

    // Cables internos para separar y unir los buses
    sc_signal<bool> sig_a[4], sig_b[4], sig_res[4];

    // Proceso para desempaquetar los buses en cables individuales
    void split_buses() {
        for(int i = 0; i < 4; i++) {
            sig_a[i].write(a.read()[i]);
            sig_b[i].write(b.read()[i]);
        }
    }

    // Proceso para empaquetar los cables individuales en el bus de salida
    void merge_buses() {
        sc_uint<4> temp;
        for(int i = 0; i < 4; i++) {
            temp[i] = sig_res[i].read();
        }
        result.write(temp);
    }

    // Constructor (La placa de circuito impreso)
    SC_CTOR(BitwiseAnd) {
        // Fabricamos las 4 compuertas AND individuales
        gate0 = new AndGate("gate0");
        gate1 = new AndGate("gate1");
        gate2 = new AndGate("gate2");
        gate3 = new AndGate("gate3");

        // 2. Conectamos los cables del Bit 0
        gate0->a(sig_a[0]); 
        gate0->b(sig_b[0]); 
        gate0->y(sig_res[0]);

        // 3. Conectamos los cables del Bit 1
        gate1->a(sig_a[1]); 
        gate1->b(sig_b[1]); 
        gate1->y(sig_res[1]);

        // 4. Conectamos los cables del Bit 2
        gate2->a(sig_a[2]); 
        gate2->b(sig_b[2]); 
        gate2->y(sig_res[2]);

        // 5. Conectamos los cables del Bit 3
        gate3->a(sig_a[3]); 
        gate3->b(sig_b[3]); 
        gate3->y(sig_res[3]);

        // Registramos los procesos de cableado
        SC_METHOD(split_buses);
        sensitive << a << b;

        SC_METHOD(merge_buses);
        sensitive << sig_res[0] << sig_res[1] << sig_res[2] << sig_res[3];
    }

    // Destructor para limpiar la memoria
    ~BitwiseAnd() {
        delete gate0; 
        delete gate1; 
        delete gate2; 
        delete gate3;
    }
};

#endif // BITWISE_AND_H