#ifndef HALF_ADDER_H
#define HALF_ADDER_H
#include <systemc.h>

SC_MODULE(HalfAdder) {
	// Puertos de 1 bit
    sc_in<bool> a, b;
    sc_out<bool> sum, cout;

    void operate_logic() {
        // Lógica de compuertas a nivel de bits
        sum.write(a.read() ^ b.read());   // Compuerta XOR
        cout.write(a.read() & b.read());  // Compuerta AND
    }

    SC_CTOR(HalfAdder) {
        SC_METHOD(operate_logic);
        sensitive << a << b; // Reacciona a cambios en A o B
    }
};
#endif