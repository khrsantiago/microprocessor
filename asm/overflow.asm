; overflow.asm - Prueba de la bandera de Overflow (V/O) en 16 bits.
; Demuestra cómo el procesador detecta cuando una operación sobrepasa los límites firmados.
; Rango firmado 16 bits: -32,768 a 32,767 (0x7FFF).

; --- Paso 1: Construcción del valor 32,767 en R0 ---
; Como LDI solo carga 8 bits, usamos desplazamientos (ADD R0, R0) para generar el valor.
LDI R0, 127
LDI R1, 127
ADD R0, R1      ; R0 = 254
ADD R0, R0      ; R0 = 508
ADD R0, R0      ; R0 = 1016
ADD R0, R0      ; R0 = 2032
ADD R0, R0      ; R0 = 4064
ADD R0, R0      ; R0 = 8128
ADD R0, R0      ; R0 = 16256
ADD R0, R0      ; R0 = 32512
LDI R1, 255
ADD R0, R1      ; R0 = 32512 + 255 = 32,767 (Máximo positivo 0x7FFF)

OUT R0          ; Muestra 32,767 en el display

; --- Paso 2: Provocar el Overflow ---
LDI R1, 1       ; Constante 1
ADD R0, R1      ; Operación: 32,767 + 1 = 32,768.
                ; En hardware de 16 bits, esto resulta en 0x8000, 
                ; que se interpreta como -32,768 (el valor más pequeño negativo).
                ; Esto activa la bandera de Overflow en el procesador.

OUT R0          ; Debería mostrar -32,768
HLT             ; Finaliza la prueba
