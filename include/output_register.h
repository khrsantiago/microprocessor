#ifndef OUTPUT_REGISTER_H
#define OUTPUT_REGISTER_H

#include <systemc.h>
#include <iostream>

// Modulo del Registro de Salida - La voz de la maquina
SC_MODULE(OutputRegister) {
    sc_in<bool> clk;
    sc_in<bool> load;           // Señal de la Unidad de Control (OUT_load)
    sc_in<sc_lv<8>> data_in;    // Datos del Bus Central
    sc_out<sc_uint<8>> display; // Salida continua a la pantalla del sistema

    bool enable_console_output;
    sc_uint<8> internal_reg;

    // Proceso para capturar datos en el flanco del reloj
    void update_logic() {
        if (clk.posedge()) {
            if (load.read() == true) {
                // Verifica si el bus tiene datos validos (no Z o X)
                if (data_in.read().is_01()) {
                    internal_reg = data_in.read().to_uint();
                    display.write(internal_reg);
                }
            }
        }
    }

    // Proceso para imprimir en la terminal cuando cambie el valor visualizado
    void monitor_output() {
        if (!enable_console_output) return;

        int8_t signed_val = (int8_t)display.read().to_uint();
        std::string green = "\033[1;32m";
        std::string reset = "\033[0m";
        
        std::stringstream ss;
        ss << "║ [DISPLAY] >>> Output: " << std::dec << (int)signed_val << " (Signed) | "
           << display.read().to_uint() << " (Unsig) | 0x" 
           << std::hex << std::uppercase << display.read().to_uint() << " (Hex) <<<";
        std::string line = ss.str();
        int target_width = 76; 
        int padding = target_width - (int)line.length();
        if (padding < 0) padding = 0;

        std::cout << "\n" << green << "╔═══════════════════════════════════════════════════════════════════════════╗" << reset << "\n";
        std::cout << green << line << std::string(padding, ' ') << " ║" << reset << "\n";
        std::cout << green << "╚═══════════════════════════════════════════════════════════════════════════╝" << reset << "\n" << std::dec;
    }

    SC_CTOR(OutputRegister) {
        internal_reg = 0;
        enable_console_output = true;
        
        SC_METHOD(update_logic);
        sensitive << clk.pos();

        SC_METHOD(monitor_output);
        sensitive << display; // Se activa solo cuando la salida cambia realmente
    }
};

#endif