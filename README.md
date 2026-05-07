# KHR-16: Microprocesador Segmentado de 16-bits en SystemC

Este proyecto implementa el microprocesador **KHR-16**, una arquitectura GPR (General Purpose Registers) de 16 bits completamente segmentada (pipelined), implementada en C++ utilizando el framework **SystemC** a nivel RTL.

El KHR-16 evoluciona desde un diseño minimalista hacia una arquitectura moderna con Load-Store, permitiendo ejecutar programas complejos con alta eficiencia gracias a su pipeline de 4 etapas y mecanismos de forwarding de datos.

## 🚀 Características Principales (v16.0)

1. **Arquitectura de 16-bits**: Registros, ALU y buses de datos operan con 16 bits completos, permitiendo aritmética de precisión y direccionamiento amplio.
2. **Pipeline de 4 Etapas**: La ejecución se divide en Fetch (IF), Decode (ID), Execute (EX) y Write-Back (WB).
3. **Mecanismo de Forwarding**: Incluye lógica de cortocircuito (EX->ID y WB->ID) para resolver hazards de datos y mantener el pipeline lleno.
4. **GPR Design**: Utiliza un Register File con 4 registros de propósito general (R0-R3).
5. **Memoria Expandida**: Direccionamiento de 8 bits para una RAM de 256 palabras de 16 bits.
6. **Entorno Visual**: Dashboard interactivo en tiempo real para observar el flujo de instrucciones y el estado interno del procesador.

## ⚙️ Conjunto de Instrucciones (ISA KHR-16)

| Familia | Mnemónico | OpCode (Hex) | Descripción |
| :--- | :--- | :--- | :--- |
| **Memoria** | `LD Rd, [Addr]` | `0x10-13` | Carga RAM[Addr] en el registro Rd. |
| **Inmediato**| `LDI Rd, Imm` | `0x20-23` | Carga un valor inmediato de 16 bits en Rd. |
| **Memoria** | `ST [Addr], Rs` | `0x30-33` | Guarda el contenido de Rs en RAM[Addr]. |
| **Indirecto**| `LD Rd, [Rs]` | `0x40-43` | Carga RAM[Rs] en Rd (Punteros). |
| **Aritmética**| `ADD Rd, Rs` | `0x60` | Rd = Rd + Rs. |
| **Aritmética**| `SUB Rd, Rs` | `0x70` | Rd = Rd - Rs. |
| **Mover** | `MOV Rd, Rs` | `0x61` | Rd = Rs. |
| **Saltos** | `JMP Addr` | `0x80` | Salto incondicional. |
| **Saltos** | `JZ/JC/JN Addr` | `0x90/A0/B0` | Saltos condicionales (Zero, Carry, Negative). |
| **Salida** | `OUT Rs` | `0xE0-E3` | Envía el valor de Rs al display de salida. |
| **Control** | `HLT` | `0xFF` | Detiene la ejecución. |

## 🛠️ Cómo Compilar y Ejecutar

1. **Requisitos**: Linux, GNU Make, GCC y SystemC (configurado en `SYSTEMC_HOME`).
2. **Compilar**:
   ```bash
   make clean && make
   ```
3. **Ejecutar**:
   ```bash
   ./bin/asm_visualizer asm/fibo_16.asm --live
   ```

## 📊 Visualizador de Pipeline
El comando `--live` activa un dashboard que muestra:
- Estado de los registros R0-R3 y Banderas.
- Contenido de cada etapa del pipeline en tiempo real.
- Historial de resultados y salida del sistema.
