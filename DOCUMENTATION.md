# Documentación Técnica Completa: Microprocesador KHR-16

## 1. Descripción General
El **KHR-16** es un microprocesador de 16 bits con arquitectura segmentada (pipelined) de **4 etapas**, impulsado por el kernel de simulación **SystemC**. Diseñado con una arquitectura Harvard y un modelo de registros de propósito general (GPR), el KHR-16 equilibra la simplicidad educativa con características avanzadas de hardware moderno como el forwarding de datos y resolución de riesgos en tiempo real.

## 2. Estructura Interna y Módulos Avanzados
El procesador se divide en bloques funcionales altamente modulares:

*   **Program Counter (PC)**: Registro de 16 bits con lógica de control de flujo. Soporta incremento secuencial, carga paralela (saltos) y habilitación/detención (enable/stall).
*   **Register File (RF)**: Contiene 4 registros de 16 bits (`R0`-`R3`). Implementa **Lógica de Write-Through**: si se escribe y lee el mismo registro en el mismo ciclo, el dato escrito se transfiere directamente a la salida de lectura para evitar riesgos RAW (Read-After-Write) internos.
*   **ALU (Unidad Aritmético-Lógica)**: Procesador de 16 bits que utiliza una arquitectura de ejecución paralela para calcular resultados aritméticos y lógicos simultáneamente, seleccionando la salida final mediante un MUX de 4 a 1.
*   **Memorias (ROM/RAM/VRAM)**:
    *   **ROM**: 256 x 16 bits (Solo lectura).
    *   **RAM**: 256 x 16 bits (Lectura asíncrona, escritura síncrona).
    *   **VRAM**: 256 x 16 bits (Mapeada para gráficos 16x16).
*   **Registros de Pipeline**:
    *   **IF/ID**: Incluye lógica de **Extensión de Signo** selectiva para la instrucción `LDI`.
    *   **ID/EX**: Buffer de señales de control y operandos.
    *   **EX/WB**: Almacena resultados de ALU y datos de memoria.

## 3. Arquitectura del Pipeline y Gestión de Riesgos
El procesador utiliza un pipeline de **4 etapas**:

1.  **Fetch (IF)**: Obtención de instrucción.
2.  **Decode (ID)**: Decodificación, lectura de RF y **Resolución de Saltos**. Los saltos se deciden aquí usando banderas adelantadas para minimizar la penalización.
3.  **Execute (EX)**: Cálculo en ALU y gestión de direcciones de memoria.
4.  **Write-Back (WB)**: Consolidación de resultados y escritura final.

### Mecanismos de Forwarding (Anticipación)
Para maximizar el rendimiento, el datapath implementa múltiples rutas de adelantamiento:
*   **EX -> ID**: Resultados de la ALU se envían al decodificador.
*   **WB -> ID**: Datos de memoria o ALU finalizados se envían al decodificador.
*   **WB -> EX**: Resultados de carga de memoria (LD) se envían a la ALU para instrucciones de almacenamiento (ST).
*   **Forwarding de Banderas (Flags)**: Las banderas Z, C, N, V de la etapa EX se anticipan a la etapa ID para permitir saltos condicionales sin detener el pipeline.

## 4. Conjunto de Instrucciones (ISA)
### Sintaxis del Ensamblador: Etiquetas
El ensamblador de KHR-16 soporta el uso de **Etiquetas (Labels)** para facilitar el flujo de control y la legibilidad del código:
*   **Definición**: Se definen colocando un nombre seguido de dos puntos (ej. `Bucle:`).
*   **Referencia**: Pueden usarse como el operando de cualquier instrucción de salto (`JMP`, `JZ`, `JC`, `JN`).
*   **Resolución**: El ensamblador calcula automáticamente la dirección de memoria exacta de la etiqueta durante la compilación, eliminando la necesidad de calcular direcciones manuales.

Ejemplo:
```asm
Inicio:
    LDI R0, 1
    OUT R0
    JMP Inicio  ; Bucle infinito usando etiquetas
```

### Tabla de Instrucciones (ISA)
| Mnemónico | OpCode | Operación | Tipo |
| :--- | :--- | :--- | :--- |
| **NOP** | `0x00` | Ninguna | Control |
| **LD Rd, [Addr]** | `0x10-13` | Rd = RAM[Addr] | Memoria |
| **LDI Rd, Imm** | `0x20-23` | Rd = Imm (Extended) | Inmediato |
| **ST [Addr], Rs** | `0x30-33` | RAM[Addr] = Rs | Memoria |
| **LD Rd, [Rs]** | `0x40-43` | Rd = RAM[Rs] | Indirecto |
| **ST [Rd], Rs** | `0x50-53` | RAM[Rd] = Rs | Indirecto |
| **ADD Rd, Rs** | `0x60` | Rd = Rd + Rs | ALU |
| **MOV Rd, Rs** | `0x61` | Rd = Rs | Registro |
| **SUB Rd, Rs** | `0x70` | Rd = Rd - Rs | ALU |
| **AND Rd, Rs** | `0x60*` | Rd = Rd & Rs | ALU (Lógica) |
| **OR Rd, Rs** | `0x60*` | Rd = Rd \| Rs | ALU (Lógica) |
| **Saltos (JMP, JZ, JC, JN)** | `0x80-B0` | PC = Addr | Control |
| **OUT Rs** | `0xE0-E3` | Display = Rs | I/O |
| **HLT** | `0xFF` | Detiene el procesador. | Control |

*\*Nota: Las instrucciones lógicas usan el mismo formato de operando que ADD/SUB con selectores internos en la unidad de control.*

## 5. Diseño de la GPU y Visualización
*   **VRAM (0x0100 - 0x01FF)**: Espacio dedicado para la matriz de 16x16 píxeles.
*   **Color RGB565**: El bit path de 16 bits permite colores reales (5 bits Rojo, 6 Verde, 5 Azul).
*   **Sincronización**: La actualización de pantalla es dirigida por la instrucción `OUT`, que actúa como un "Vertical Sync" de software para evitar parpadeos en la simulación.

## 6. Depuración y Trazado (Tracing)
El KHR-16 expone múltiples puertos de depuración internos:
*   **Debug Registers**: Los registros `R0`-`R3` son visibles en tiempo real durante la simulación.
*   **Pipeline Trace**: Señales de PC y OpCodes para todas las etapas (IF, ID, EX, WB).
*   **Bus Monitoring**: Monitoreo del bus común y del bus de direcciones de memoria (MAR).

## 7. Entorno de Simulación
El proyecto se compila usando **SystemC 3.0.2** y **C++17**:
1.  **asm_runner**: Ejecución rápida por consola con volcado de registros por ciclo.
2.  **asm_visualizer**: Interfaz interactiva para depuración paso a paso.
3.  **asm_screen**: Motor gráfico para aplicaciones visuales (Juegos, animaciones).

---
*Diseño y Documentación por Khristopher Santiago - Arquitectura de Computadores.*
