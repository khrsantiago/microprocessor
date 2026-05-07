; program.asm - Test de regresion KHR-16
LDI R0, 7     ; R0 = 7
ST [15], R0   ; Memoria[15] = 7
LDI R1, 3     ; R1 = 3
ADD R0, R1    ; R0 = 7 + 3 = 10
OUT R0        ; Display = 10
HLT           ; Fin
