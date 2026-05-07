#ifndef VRAM_H
#define VRAM_H

#include <systemc.h>

// Modulo de Memoria de Video (VRAM) dedicada
// Proporciona un buffer de 256 palabras para graficos (16x16)
SC_MODULE(VRAM) {
    sc_in<bool> clk;
    sc_in<bool> we;
    sc_in<sc_uint<16>> addr;         // Bus de 16 bits (Aunque solo usamos 8 bits inferiores para los 256 pixeles)
    sc_in<sc_uint<16>> data_in;
    sc_out<sc_uint<16>> data_out;

    sc_uint<16> memory[256];

    void read_data() {
        // En VRAM solo nos importan las direcciones 256-511 (0x100-0x1FF)
        // Pero el decodificador de ComputerTop ya nos mandara la direccion ajustada (0-255)
        uint16_t idx = addr.read().to_uint() & 0xFF; // Mascara de seguridad
        data_out.write(memory[idx]);
    }

    void write_data() {
        if (clk.posedge()) {
            if (we.read() == 1) {
                uint16_t idx = addr.read().to_uint() & 0xFF;
                memory[idx] = data_in.read();
            }
        }
    }

    SC_CTOR(VRAM) {
        for (int i = 0; i < 256; i++) {
            memory[i] = 0;
        }

        SC_METHOD(read_data);
        sensitive << addr << we;

        SC_METHOD(write_data);
        sensitive << clk.pos();
    }
};

#endif // VRAM_H
