#ifndef MAR_H
#define MAR_H

#include <systemc.h>

// Memory Address Register (MAR) - 4 bits
SC_MODULE(MemoryAddressRegister) {
    sc_in<bool> clk;
    sc_in<bool> ld;              // Habilitador de carga (mar_in de la CU)
    sc_in<sc_lv<4>> data_in;     // Conectado directamente al Bus Central
    sc_out<sc_uint<4>> addr_out; // Salida estable hacia la RAM

    sc_uint<4> current_addr;

    void update_logic() {
        if (clk.posedge()) {
            // Si la Unidad de Control ordena capturar la direccion
            if (ld.read() == true) {
                sc_lv<4> bus_val = data_in.read();
                
                // Solo capturamos si el bus tiene voltajes validos (0 o 1)
                // Esto ignora el estado Hi-Z (Z) para no corromper la direccion
                if (bus_val.is_01()) {
                    current_addr = bus_val.to_uint();
                }
            }
        }
        // La RAM siempre ve lo que hay en el registro de forma asincrona
        addr_out.write(current_addr);
    }

    SC_CTOR(MemoryAddressRegister) {
        current_addr = 0;
        SC_METHOD(update_logic);
        sensitive << clk.pos();
    }
};

#endif