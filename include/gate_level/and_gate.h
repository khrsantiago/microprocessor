#ifndef AND_GATE_H
#define AND_GATE_H

#include <systemc.h>

// Modulo de compuerta AND de 1 bit
SC_MODULE(AndGate) {
    // Puertos individuales
    sc_in<bool> a;
    sc_in<bool> b;
    sc_out<bool> y;

    // Comportamiento de la compuerta fisica
    void execute() {
        y.write(a.read() & b.read());
    }

    // Constructor
    SC_CTOR(AndGate) {
        SC_METHOD(execute);
        sensitive << a << b;
    }
};

#endif // AND_GATE_H