#ifndef ADDER_H
#define ADDER_H
#include <systemc.h>
#include "full_adder.h"

SC_MODULE(Adder4Bits) {
    // Puertos externos (Buses)
    sc_in<sc_uint<4>> a, b;
    sc_in<bool> cin;
    sc_out<sc_uint<4>> sum;
    sc_out<bool> cout;

    // Instancias de nuestros submódulos (Los 4 chips sumadores completos)
    FullAdder *fa0, *fa1, *fa2, *fa3;

    // Cables internos para propagar el acarreo (Ripple Carry)
    sc_signal<bool> c1, c2, c3; 
    
    // Cables internos para desempaquetar los buses de 4 bits
    sc_signal<bool> sig_a[4], sig_b[4], sig_sum[4];

    // Proceso para separar el Bus principal en cables de 1 bit
    void split_buses() {
        for(int i=0; i<4; i++) {
            sig_a[i].write(a.read()[i]);
            sig_b[i].write(b.read()[i]);
        }
    }

    // Proceso para agrupar los cables de 1 bit en el Bus de salida
    void join_buses() {
        sc_uint<4> temp;
        for(int i=0; i<4; i++) {
            temp[i] = sig_sum[i].read();
        }
        sum.write(temp);
    }

    SC_CTOR(Adder4Bits) {
        // 1. Fabricamos los 4 chips
        fa0 = new FullAdder("fa0");
        fa1 = new FullAdder("fa1");
        fa2 = new FullAdder("fa2");
        fa3 = new FullAdder("fa3");

        // 2. Conectamos los cables del Bit 0 (El menos significativo)
        fa0->a(sig_a[0]); fa0->b(sig_b[0]); fa0->cin(cin);
        fa0->sum(sig_sum[0]); fa0->cout(c1); // El acarreo sale por el cable c1

        // 3. Conectamos los cables del Bit 1
        fa1->a(sig_a[1]); fa1->b(sig_b[1]); fa1->cin(c1); // Recibe el acarreo de fa0
        fa1->sum(sig_sum[1]); fa1->cout(c2);

        // 4. Conectamos los cables del Bit 2
        fa2->a(sig_a[2]); fa2->b(sig_b[2]); fa2->cin(c2);
        fa2->sum(sig_sum[2]); fa2->cout(c3);

        // 5. Conectamos los cables del Bit 3 (El más significativo)
        fa3->a(sig_a[3]); fa3->b(sig_b[3]); fa3->cin(c3);
        fa3->sum(sig_sum[3]); fa3->cout(cout); // El último acarreo va al pin de salida exterior

        // Registramos los procesos de manejo de buses
        SC_METHOD(split_buses);
        sensitive << a << b;

        SC_METHOD(join_buses);
        sensitive << sig_sum[0] << sig_sum[1] << sig_sum[2] << sig_sum[3];
    }
    
    // Destructor para limpiar la memoria
    ~Adder4Bits() {
        delete fa0; delete fa1; delete fa2; delete fa3;
    }
};
#endif