; sub.asm - Prueba de restas (Positiva y Negativa)
; Demuestra el uso de la instrucción SUB y cómo el procesador gestiona resultados negativos.

; --- 1. Resta positiva: 10 - 7 = 3 ---
LDI R0, 10    ; Carga el valor 10 en el registro R0 (Minuendo)
LDI R1, 7     ; Carga el valor 7 en el registro R1 (Sustraendo)
SUB R0, R1    ; R0 = R0 - R1 -> R0 = 10 - 7. Resultado: 3
OUT R0        ; Envía el resultado (3) al registro de salida para visualización

; --- 2. Resta negativa: 7 - 10 = -3 ---
LDI R0, 7     ; Carga el valor 7 en R0 (Minuendo)
LDI R1, 10    ; Carga el valor 10 en R1 (Sustraendo)
SUB R0, R1    ; R0 = R0 - R1 -> R0 = 7 - 10. Resultado: -3
OUT R0        ; Envía el resultado al registro de salida.
              ; En el visualizador se verá como -3 (signed) o 0xFFFD (hex).

HLT           ; Finaliza la ejecución del programa
