; sub.asm - Prueba de restas (Positiva y Negativa)

; 1. Resta positiva: 10 - 7 = 3
LDI R0, 10
LDI R1, 7
SUB R0, R1
OUT R0       ; Debería mostrar 3

; 2. Resta negativa: 7 - 10 = -3
LDI R0, 7
LDI R1, 10
SUB R0, R1
OUT R0       ; Debería mostrar -3 (o 253 en formato sin signo)

HLT          ; Fin del programa
