# Documentación Técnica: Microprocesador KHR-8 (Arquitectura de 8-bits)

Este documento proporciona una visión detallada del diseño, funcionamiento y programación del microprocesador KHR-8 modificado para soportar un entorno gráfico (GPU/VRAM) y un Juego de la Vida de Conway.

---

## 1. Módulos del Sistema

El procesador está construido utilizando **SystemC** y sigue una arquitectura modular donde cada componente representa una unidad lógica del hardware.

| Módulo | Función Principal |
| :--- | :--- |
| **ProgramCounter (PC)** | Almacena la dirección de la siguiente instrucción a ejecutar. Se incrementa automáticamente o salta según las instrucciones de control. |
| **InstructionRegister (IR)** | Captura el OpCode (8 bits) y el Operando (8 bits) del bus. Envía el OpCode a la Unidad de Control. |
| **MemoryAddressRegister (MAR)** | Sostiene la dirección de memoria que la RAM debe leer o escribir en el ciclo actual. |
| **RAM (Memoria)** | Almacenamiento de 256 palabras (bytes). Contiene tanto el código del programa como los datos y la VRAM. |
| **Accumulator (Reg A)** | Registro principal para operaciones aritméticas. Es el destino por defecto de la mayoría de las operaciones. |
| **Register B** | Registro secundario que sostiene el segundo operando para la ALU (ej. en sumas o restas). |
| **Register X (Índice)** | Registro especializado para direccionamiento indirecto (punteros). Permite acceder a `RAM[X]`. |
| **ALU (Unidad Aritmética)** | Realiza sumas y restas, y gestiona las banderas de Zero (Z) y Carry (C). |
| **ControlUnit (CU)** | El "cerebro" del procesador. Es una máquina de estados (FSM) que decodifica las instrucciones y genera las señales de control. |
| **OutputRegister** | Actúa como interfaz con el exterior, permitiendo visualizar valores en el display. |
| **TriStateBuffer** | Controla el flujo de datos hacia el bus central, evitando colisiones al asegurar que solo un módulo escriba a la vez. |

---

## 2. Banderas del Procesador (Flags)

El procesador cuenta con un registro de estado que almacena "banderas" para indicar condiciones internas después de una operación aritmética o lógica. Estas son fundamentales para los saltos condicionales.

| Bandera | Siglas | Origen | Descripción |
| :--- | :--- | :--- | :--- |
| **Zero Flag** | **Z** | ALU / Registro A | Se activa (1) si el resultado de la última operación fue exactamente **0**. Se usa para la instrucción `JZ`. |
| **Carry Flag** | **C** | ALU | Se activa (1) si hubo un **acarreo** (en suma) o un **préstamo** (en resta). Indica que el resultado desbordó los 8 bits. Se usa para `JC`. |

> [!NOTE]
> Aunque el sistema tiene señales preparadas para **Overflow** (V) y **Negativo** (N), actualmente la lógica del procesador se centra en Z y C, que son las necesarias para el algoritmo de Conway y cálculos básicos.

---

## 3. Set de Instrucciones (ISA)

El procesador utiliza instrucciones de **16 bits**: 8 bits para el código de operación (OpCode) y 8 bits para el operando.

| Mnemónico | OpCode | Descripción |
| :--- | :--- | :--- |
| **NOP** | `0x0` | No hace nada. |
| **LDA [addr]** | `0x1` | Carga el valor en `RAM[addr]` al Acumulador A. |
| **ADD [addr]** | `0x2` | Suma el valor de `RAM[addr]` al Acumulador A. |
| **SUB [addr]** | `0x3` | Resta el valor de `RAM[addr]` del Acumulador A. |
| **LDI [val]** | `0x4` | Carga un valor inmediato al Acumulador A. |
| **STA [addr]** | `0x5` | Guarda el valor del Acumulador A en `RAM[addr]`. |
| **TAX** | `0x6` | Transfiere el valor de A al Registro X. |
| **TXA** | `0x7` | Transfiere el valor de X al Acumulador A. |
| **JMP [addr]** | `0x8` | Salto incondicional a la dirección `addr`. |
| **JZ [addr]** | `0x9` | Salta a `addr` si la bandera Zero está activa. |
| **JC [addr]** | `0xA` | Salta a `addr` si la bandera Carry está activa. |
| **LDA_X** | `0xB` | Carga al Acumulador A el valor de `RAM[X]` (Puntero). |
| **STA_X** | `0xC` | Guarda el valor de A en `RAM[X]` (Puntero). |
| **OUT** | `0xE` | Envía el valor de A al registro de salida (Dispara V-Sync en la GPU). |
| **HLT** | `0xF` | Detiene el procesador. |

---

## 4. Punteros y Direccionamiento Dinámico

Una característica avanzada de este procesador es el uso del **Registro X** como puntero.
- Mientras que `LDA 15` siempre lee la dirección 15, `LDA_X` lee la dirección contenida en X.
- **Ejemplo**: Si X tiene el valor 231, ejecutar `LDA_X` leerá la primera celda de la VRAM.
- Esto es fundamental para algoritmos como Conway, donde necesitamos movernos por una matriz de datos de forma dinámica.

---

## 5. Subsistema de Video (GPU) y Conway

### La VRAM
El procesador reserva un bloque de memoria para la visualización:
- **Direcciones 231 a 255**: Bloque de 25 bytes que representa una cuadrícula de **5x5**.
- Cualquier valor mayor a 0 en estas direcciones se considera una "Célula Viva" (█).
- Un valor de 0 se considera una "Célula Muerta" (░).

### Funcionamiento de la GPU
La GPU no es un hardware separado, sino una interpretación lógica del visualizador (`main_screen.cpp`):
1. El programa procesa la lógica del Juego de la Vida en un buffer paralelo (direcciones 206-230).
2. Una vez calculado el siguiente estado, lo copia a la VRAM (231-255).
3. La instrucción **OUT** actúa como un interruptor de **Sincronización Vertical (V-Sync)**, indicando al simulador que debe refrescar la pantalla de la consola.

---

## 6. Programas en Ensamblador (.asm)

El proyecto incluye varios programas de ejemplo:

| Programa | Descripción |
| :--- | :--- |
| **conway.asm** | Implementación completa del Juego de la Vida. Calcula vecinos, aplica reglas de supervivencia y actualiza la pantalla. |
| **fibo.asm** | Genera la secuencia de Fibonacci en el registro de salida. |
| **mult.asm** | Realiza la multiplicación de dos números mediante sumas sucesivas. |
| **div_sqrt.asm** | Ejemplos de algoritmos más complejos como división y raíz cuadrada. |
| **blinker.asm** | Un patrón simple de Conway que oscila eternamente. |

---

## 7. Consejos para Estudiantes

- **Depuración**: Observa las trazas en la consola para ver qué micro-estados (T1, T2...) está ejecutando la Unidad de Control.
- **Memoria**: Recuerda que las direcciones 0-149 son típicamente para código, mientras que las direcciones superiores se reservan para datos y VRAM.
- **Ciclos de Reloj**: Cada instrucción consume un número diferente de ciclos (T-States). Una instrucción `LDA_X` es más compleja que una `NOP`.

---
*Documento generado para el curso de Arquitectura de Computadores - ULA.*
