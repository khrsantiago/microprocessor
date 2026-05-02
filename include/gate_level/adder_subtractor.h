#ifndef ADDER_SUBTRACTOR_H
#define ADDER_SUBTRACTOR_H

#include <systemc.h>
#include "adder.h" // ¡Importamos el chip que fabricaste antes!

/**
 * @class adder_subtractor
 * @brief Unidad Aritmética combinada. op=0 (Suma), op=1 (Resta mediante Complemento a 2)
 */
SC_MODULE(AdderSubtractor) {
    // --- Puertos externos ---
    sc_in<sc_uint<4>> a, b;
    sc_in<bool> op; // Señal de control: 0 = Suma, 1 = Resta
    
    sc_out<sc_uint<4>> result;
    sc_out<bool> cout;

    // Bandera
    sc_out<bool> zero;     // Z-Flag (Para indicar resultado 0)
    sc_out<bool> negative; // N-Flag (Para indicar resultado negativo)
    sc_out<bool> overflow; // V-Flag (Para indicar desbordamiento)

    // --- Componente interno ---
    Adder4Bits *adder_4bits;

    // --- Cables internos ---
    sc_signal<sc_uint<4>> processed_b;
    sc_signal<sc_uint<4>> internal_res; // Señal interna para analizar el resultado

    // --- Lógica de Inversión (Complemento a 1) ---
    void process_b() {
        if (op.read() == 1) {
            // Si es resta, invertimos todos los bits de B (Ej: 0011 -> 1100)
            processed_b.write(~b.read());
        } else {
            // Si es suma, el cable pasa directo sin modificaciones
            processed_b.write(b.read());
        }

    }

    // Lógica para calcular las banderas
    void calculate_flags() {
        // Leemos los valores actuales de A, B procesado y el resultado interno
        sc_uint<4> val_a = a.read();
        sc_uint<4> val_b_p = processed_b.read();
        sc_uint<4> res = internal_res.read();
        
        // 1. Lógica de la Bandera de Cero (Z)
        // Se activa si el resultado es 0000
        zero.write(res == 0);

        // 2. Lógica de la Bandera de Negativo (N)
        // Simplemente miramos el bit 3 (el MSB)
        negative.write(res[3]);

        // Lógica de Overflow (V):
        // (A_msb == B_msb) AND (A_msb != Res_msb)
        bool v = (val_a[3] == val_b_p[3]) && (val_a[3] != res[3]);
        overflow.write(v);
        
        // Enviamos el resultado al puerto de salida
        result.write(res);
    }

    // --- Constructor ---
    SC_CTOR(AdderSubtractor) {
        // 1. Fabricamos el sumador interno
        adder_4bits = new Adder4Bits("adder_4bits");

        // 2. Conectamos los cables
        adder_4bits->a(a);
        adder_4bits->b(processed_b); // B modificado (normal o invertido)
        
        // ¡LA MAGIA OCURRE AQUÍ!
        // Conectamos el pin de control 'op' directamente al acarreo de entrada (Cin)
        // Si op=0, suma (Cin=0). Si op=1, resta y automáticamente inyecta el +1 del complemento a 2.
        adder_4bits->cin(op); 

        adder_4bits->sum(internal_res); // Conectamos a la señal interna
        
        adder_4bits->cout(cout);        // El acarreo final sale directo

        // 3. Registramos el proceso combinacional
        SC_METHOD(process_b);
        sensitive << b << op;

        SC_METHOD(calculate_flags);
        sensitive << internal_res << a << processed_b;
    }

    ~AdderSubtractor() {
        delete adder_4bits;
    }
};

#endif // ADDER_SUBTRACTOR_H