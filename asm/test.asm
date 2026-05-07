; test.asm - Prueba funcional básica de la arquitectura KHR-16 GPR
; Verifica la carga de inmediatos, suma, almacenamiento y carga desde RAM.

; --- Prueba de Aritmética y Memoria ---
LDI R1, 10      ; R1 = 10 (Usando LDI - Load Immediate)
LDI R2, 7       ; R2 = 7
ADD R1, R2      ; R1 = R1 + R2 -> 10 + 7 = 17
ST 15, R1       ; RAM[15] = R1 (Guardar el resultado 17 en la dirección 15)

; --- Verificación de Persistencia ---
LDI R1, 0       ; Limpiamos R1 para asegurar que la carga posterior funcione
LD R1, 15       ; R1 = RAM[15] (Cargar el valor 17 de vuelta al registro)

; --- Salida ---
OUT R1          ; Debería mostrar 17 en el registro de salida
HLT             ; Finalizar programa
