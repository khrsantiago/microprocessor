#ifndef FULL_ADDER_H
#define FULL_ADDER_H  
#include <systemc.h>
#include "half_adder.h" // Importamos el chip base

SC_MODULE(FullAdder) {
    // --- Puertos externos ---
    sc_in<bool> a, b, cin;
    sc_out<bool> sum, cout;

    // --- Instancias de chips internos ---
    HalfAdder *ha1;
    HalfAdder *ha2;

    // --- Cables internos ---
    sc_signal<bool> sum1;   // Conecta la salida de HA1 con la entrada de HA2
    sc_signal<bool> carry1; // Acarreo del primer medio sumador
    sc_signal<bool> carry2; // Acarreo del segundo medio sumador

    // --- Proceso para la Compuerta OR final ---
    void or_logic_carry() {
        cout.write(carry1.read() | carry2.read());
    }

    // --- Constructor (Nuestra placa de soldadura) ---
    SC_CTOR(FullAdder) {
        // 1. Fabricamos los dos Medios Sumadores
        ha1 = new HalfAdder("ha1");
        ha2 = new HalfAdder("ha2");

        // 2. Conectamos el Medio Sumador 1
        ha1->a(a);            // Pin 'a' del chip al puerto 'a' exterior
        ha1->b(b);            // Pin 'b' del chip al puerto 'b' exterior
        ha1->sum(sum1);       // La suma va al cable interno 'sum1'
        ha1->cout(carry1);    // El acarreo va al cable interno 'carry1'

        // 3. Conectamos el Medio Sumador 2
        ha2->a(sum1);         // Pin 'a' del chip recibe el cable 'sum1'
        ha2->b(cin);          // Pin 'b' del chip recibe el 'cin' exterior
        ha2->sum(sum);        // La suma va directo al puerto 'sum' exterior
        ha2->cout(carry2);    // El acarreo va al cable interno 'carry2'

        // 4. Configuramos la compuerta OR final
        SC_METHOD(or_logic_carry);
        sensitive << carry1 << carry2; // Se ejecuta si cambia algún acarreo interno
    }

    // --- Destructor ---
    ~FullAdder() {
        // Liberamos la memoria de los chips instanciados
        delete ha1;
        delete ha2;
    }
};
#endif