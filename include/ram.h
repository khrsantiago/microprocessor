#ifndef RAM_H
#define RAM_H

#include <systemc.h>

SC_MODULE(RAM) {
    // Puertos del sistema
    sc_in<bool> clk;                // Reloj (System clock)
    sc_in<bool> we;                 // Habilitador de escritura (Write Enable)
    sc_in<sc_uint<8>> addr;         // Bus de direcciones de 8 bits (Address)
    sc_in<sc_uint<8>> data_in;      // Bus de entrada de datos (Data In)
    sc_out<sc_uint<8>> data_out;    // Bus de salida de datos (Data Out)

    // El nucleo de la memoria: Una matriz de 256 posiciones, cada una de 8 bits
    sc_uint<8> memory[256];

    // Proceso 1: Lectura Asincrona (Como mirar por una ventana)
    void read_data() {
        // Leemos continuamente la direccion y exponemos el contenido de la matriz
        data_out.write(memory[addr.read()]);
    }

    // Proceso 2: Escritura Sincrona (Como abrir una caja fuerte)
    void write_data() {
        // Solo verificamos cuando ocurre el flanco de subida del reloj
        if (clk.posedge()) {
            // El "portero": Solo escribimos si Write Enable (WE) es 1
            if (we.read() == 1) {
                // Guardamos el dato de entrada en la direccion especificada
                memory[addr.read()] = data_in.read();
            }
        }
    }

    SC_CTOR(RAM) {
        // Inicializamos toda la memoria en 0 por seguridad
        for (int i = 0; i < 256; i++) {
            memory[i] = 0;
        }

        // Registramos el proceso de lectura asincrona
        SC_METHOD(read_data);
        sensitive << addr << we; // Sensible a cambios en direccion o estado

        // Registramos el proceso de escritura sincrona
        SC_METHOD(write_data);
        sensitive << clk.pos();  // ESTRICTAMENTE sensible solo al reloj
    }
};

#endif // RAM_H