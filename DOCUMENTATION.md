# Documentación Técnica: Microprocesador KHR-16

Este documento detalla la arquitectura y el funcionamiento del microprocesador **KHR-16**, un diseño segmentado de 16 bits optimizado para rendimiento y flexibilidad.

## 1. Arquitectura del Pipeline

El KHR-16 utiliza un pipeline de 4 etapas para maximizar el throughput de instrucciones:

1. **Instruction Fetch (IF)**: Obtiene la instrucción de 16 bits desde la ROM utilizando el Program Counter.
2. **Instruction Decode (ID)**: Decodifica el OpCode, lee los operandos del Register File y resuelve las dependencias de datos mediante forwarding desde etapas posteriores.
3. **Execute (EX)**: Realiza operaciones en la ALU o calcula direcciones de memoria para accesos indirectos.
4. **Write-Back (WB)**: Escribe el resultado de vuelta en el Register File o actualiza el registro de salida.

## 2. Unidades Funcionales

- **Register File**: Contiene 4 registros de 16 bits (R0, R1, R2, R3). Soporta dos lecturas y una escritura simultáneas.
- **ALU**: Unidad de 16 bits capaz de realizar suma, resta, AND y OR. Gestiona banderas de estado: Zero (Z), Carry (C), Negative (N) y Overflow (V).
- **Control Unit**: Decodificador combinacional que genera señales de control para todas las etapas del pipeline basándose en el OpCode.
- **Memoria**: ROM de instrucciones y RAM de datos (256x16 bits cada una).

## 3. Manejo de Hazards

Para evitar detenciones (stalls), el procesador implementa **Data Forwarding**:
- **EX->ID**: El resultado de la ALU se envía de vuelta a la etapa de decodificación si la siguiente instrucción depende de él.
- **WB->ID**: El valor que se está escribiendo en el Register File se envía a la etapa de decodificación si hay una coincidencia de registros.

## 4. Programación (ISA)

Las instrucciones son de 16 bits:
- **Byte 1**: OpCode (incluyendo índice de registro en los bits bajos para algunas instrucciones).
- **Byte 2**: Operando (Valor inmediato, dirección de memoria o índices de registros adicionales).

### Ejemplo de Código
```asm
LDI R0, 10    ; Carga 10 en R0
LDI R1, 5     ; Carga 5 en R1
ADD R0, R1    ; R0 = R0 + R1 (15)
OUT R0        ; Muestra 15 en el display
HLT           ; Fin
```

## 5. Visualizador
El proyecto incluye `main_visualizer.cpp`, que proporciona una interfaz interactiva para seguir la ejecución ciclo a ciclo, permitiendo una depuración profunda del flujo del pipeline y la resolución de conflictos.

---
*Diseño para el curso de Arquitectura de Computadores - ULA.*
