#ifndef ADDER_H
#define ADDER_H
#include <systemc.h>
#include "full_adder.h"

SC_MODULE(Adder8Bits) {
    // Puertos externos (Buses)
    sc_in<sc_uint<8>> a, b;
    sc_in<bool> cin;
    sc_out<sc_uint<8>> sum;
    sc_out<bool> cout;

    // Instancias de nuestros submódulos (Los 8 chips sumadores completos)
    FullAdder *fa[8];

    // Cables internos para propagar el acarreo (Ripple Carry)
    sc_signal<bool> c[7]; 
    
    // Cables internos para desempaquetar los buses de 8 bits
    sc_signal<bool> sig_a[8], sig_b[8], sig_sum[8];

    // Proceso para separar el Bus principal en cables de 1 bit
    void split_buses() {
        for(int i=0; i<8; i++) {
            sig_a[i].write(a.read()[i]);
            sig_b[i].write(b.read()[i]);
        }
    }

    // Proceso para agrupar los cables de 1 bit en el Bus de salida
    void join_buses() {
        sc_uint<8> temp;
        for(int i=0; i<8; i++) {
            temp[i] = sig_sum[i].read();
        }
        sum.write(temp);
    }

    SC_CTOR(Adder8Bits) {
        for(int i=0; i<8; i++) {
            std::string name = "fa" + std::to_string(i);
            fa[i] = new FullAdder(name.c_str());
            fa[i]->a(sig_a[i]);
            fa[i]->b(sig_b[i]);
            fa[i]->sum(sig_sum[i]);
            
            if(i == 0) fa[i]->cin(cin);
            else fa[i]->cin(c[i-1]);
            
            if(i == 7) fa[i]->cout(cout);
            else fa[i]->cout(c[i]);
        }

        // Registramos los procesos de manejo de buses
        SC_METHOD(split_buses);
        sensitive << a << b;

        SC_METHOD(join_buses);
        for(int i=0; i<8; i++) sensitive << sig_sum[i];
    }
    
    // Destructor para limpiar la memoria
    ~Adder8Bits() {
        for(int i=0; i<8; i++) delete fa[i];
    }
};
#endif