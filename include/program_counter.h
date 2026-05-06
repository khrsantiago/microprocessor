#ifndef PROGRAM_COUNTER_H
#define PROGRAM_COUNTER_H

#include <systemc.h>

// Program Counter (PC) - 8 bits (Direcciona 256 palabras de RAM)
SC_MODULE(ProgramCounter) {
    sc_in<bool> clk;            // Reloj del sistema
    sc_in<bool> reset;          // Reset asincrono (Vuelve a la instruccion 0)
    sc_in<bool> en;             // Enable (Permite avanzar a la siguiente instruccion)
    sc_in<bool> load;           // Habilitador de salto (Jump Load)
    sc_in<sc_uint<8>> data_in;  // Nueva direccion proveniente del IR
    sc_out<sc_uint<8>> q;       // Direccion de salida hacia la RAM

    // Registro interno para mantener la cuenta
    sc_uint<8> count;

    void update_logic() {
        // Prioridad 1: Reset (Vuelve al inicio)
        if (reset.read() == true) {
            count = 0;
        } 
        else if (clk.posedge()) {
            // Prioridad 2: Salto (Carga Paralela)
            // Esto actua como el multiplexor interno
            if (load.read() == true) {
                count = data_in.read(); 
            } 
            // Prioridad 3: Incremento secuencial
            else if (en.read() == true) {
                count = count + 2;
            }
        }
        q.write(count);
    }

    SC_CTOR(ProgramCounter) {
        SC_METHOD(update_logic);
        // Sensible al flanco de subida del reloj Y al flanco de subida del reset
        sensitive << clk.pos() << reset.pos();
        
        count = 0;
    }
};

#endif // PROGRAM_COUNTER_H