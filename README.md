# SAP-1 a 8-Bits: Evolución de Arquitectura en SystemC

Este proyecto es una evolución del microprocesador **SAP-1 (Simple As Possible 1)** de Albert Paul Malvino, diseñado e implementado 100% en C++ usando el framework **SystemC** a nivel RTL (Register Transfer Level).

El proyecto original limitaba el procesador a un diseño restringido de 4/8-bits (memoria de 16-bytes y bus de 8-bits), pero ha sido **evolucionado a una verdadera arquitectura de 8 bits discretos** capaz de direccionar mucha más memoria, realizar cálculos más pesados y despachar instrucciones más complejas mediante un formato Dual-Fetch.

## 🚀 Cambios de la Evolución (v2.0)

1. **Expansión Modular**: Todos los registros (`Accumulator`, `PC`, `MAR`, `IR`, `OUT` y `ALU`) y el bus de transferencia interna ahora gestionan **8 bits** completos.
2. **Capacidad de Memoria**: La RAM creció de 16 Bytes a **256 Bytes**, adoptando `sc_uint<8>` como ancho de direcciones. 
3. **Instrucciones de 16-bits (Dual-Fetch)**: Las instrucciones pasaron de empaquetarse en 1 byte (4 bits OpCode + 4 bits Operandos) a un espacioso modelo `2-Byte`. El ensamblador genera el Opcode de 8 bits en el byte `N` y el Operando de 8 bits en el byte `N+1`.
4. **Ciclo Fetch Extendido**: La Unidad de Control (`ControlUnit`) evolucionó su FSM de T1-T6 a **T1-T12**, dividiendo el ciclo Fetch original en dos sub-operaciones sincrónicas para ensamblar la instrucción doble en el Registro de Instrucciones (`InstructionRegister`).
5. **Zero Flag Latching**: Se agregó un pestillo sincrónico del Flag `Z` directamente ligado a los flancos de recarga del Acumulador para lidiar correctamente con ciclos de máquina inactivos.

## ⚙️ Conjunto de Instrucciones (ISA de 8 Bits)

| Familia | Mnemónico | OpCode (Hex) | Operando | Descripción |
| :--- | :--- | :--- | :--- | :--- |
| Control | `NOP` | `0x00` | Ninguno | No hace nada (No Operation). Desperdicia su turno. |
| Memoria | `LDA` | `0x01` | Dirección | Load A: Carga en el Reg A el valor guardado en RAM[Dirección]. |
| ALU | `ADD` | `0x02` | Dirección | Add: Suma RAM[Dirección] al valor alojado en Reg A. |
| ALU | `SUB` | `0x03` | Dirección | Resta: Resta RAM[Dirección] al valor en Reg A. |
| Inmed. | `LDI` | `0x04` | Valor (Inmediato) | Carga constante: Guarda explícitamente el valor proveido en el Reg A. |
| Memoria | `STA` | `0x05` | Dirección | Store A: Sobrescribe la celda RAM[Dirección] con el valor en Reg A. |
| I/O | `OUT` | `0x0E` | Ninguno | Ejecución Out: Refleja y extrae el valor actual del Reg A al registro pantalla. |
| Salto | `JMP` | `0x08` | Dirección | Jump incondicional: Redirige el Program Counter (PC) a la Dirección dada. |
| Salto | `JZ` | `0x09` | Dirección | Jump if Zero: Salta a la Dirección solo si la última instrucción puso en 0 el Reg A. |
| Salto | `JC` | `0x0A` | Dirección | Jump if Carry: Salta si el último cálculo generó desbordamiento (Bandera Acarreo = 1). |
| Control | `HLT` | `0x0F` | Ninguno | Halt: Detiene la simulación del procesador irreversiblemente. |

*Nota: Aunque una instrucción como `OUT` o `HLT` no dependa de operandos, siempre consume un segundo byte en memoria (`0x00`) para respetar el alineamiento simétrico de `16-bits` del Program Counter.*

## 🛠️ Cómo Compilar y Ejecutar

El proyecto viene provisto con un un pequeño cargador / ensamblador escrito en C++ moderno, un Top-Level Interconnect que une las partes y archivos `.asm` de prueba. 

1. **Requisitos**: Tu ambiente debe correr Linux y tener instalado GNU Make, GCC y SystemC (configurado en la MACRO `SYSTEMC_HOME` en el Makefile, por defecto configurado a `/usr/local/systemc-3.0.2`).
2. **Compilar proyecto**:
   Dirígete a la raíz del repositorio y corre:
   ```bash
   make clean
   make
   ```
   *Esto compilará todos los módulos y generará un ejecutable final en `bin/test_asm_runner`.*

3. **Ejecutar programas asamblados**:
   Dentro de la carpeta `asm/` hay varios programas que demuestran software de 8 bits corriendo sobre el procesador: `resta.asm`, `mult.asm`, `div.asm`, y los nuevos **`fibo_16.asm`** y **`sqrt.asm`** (que introducen Aritmética de Múltiple Precisión de 16 bits usando la bandera Carry `JC` para probar algoritmos de la vida real como buscar la raíz cuadrada restando números impares).

   Corre el motor visualizador y observa el registro paso a paso:
   ```bash
   ./bin/asm_visualizer asm/sqrt.asm --live
   ```

   **Opciones para el Dashboard Interactivo (`--live`):**
   - `--fast`: Modalidad Turbo (baja el retraso entre ciclos lógicos a tan solo 20ms).
   - `--delay=X`: Modalidad Personalizada (Ej: `--delay=500` ralentiza el sistema a medio segundo por ciclo).

   Verás un volcado asíncrono de cada estado de la máquina, los buses compartidos, las banderas Cero/Carry y un `>>> Current Machine Output: <<<` cada vez que el acumulador active `OUT`.
