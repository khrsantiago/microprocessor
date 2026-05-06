; sum.asm - Suma de 7 + 3
LDI R0, 7    ; Carga 7 en R0
LDI R1, 3    ; Carga el segundo número en R1
ADD R0, R1   ; Suma: R0 = R0 + R1 (7 + 3 = 10)
OUT R0       ; Envía el resultado (10) al display

; Suma 15 + 10
LDI R0, 15   ; Carga 15 en R0
LDI R1, 10   ; Carga el segundo número en R1
ADD R0, R1   ; Suma: R0 = R0 + R1 (15 + 10 = 25)
OUT R0       ; Envía el resultado (25) al display

; Suma -8 + 25
LDI R0, -8   ; Carga -8 en R0
LDI R1, 25   ; Carga 25 en R1
ADD R0, R1   ; Suma: R0 = R0 + R1 (-8 + 25 = 17)
OUT R0       ; Envía el resultado (17) al display

; Suma -8 + -8
LDI R0, -8   ; Carga -8 en R0
LDI R1, -8   ; Carga -8 en R1
ADD R0, R1   ; Suma: R0 = R0 + R1 (-8 + -8 = -16)
OUT R0       ; Envía el resultado (-16) al display

; Suma de 254 + 2
LDI R0, 254   ; Carga 254 en R0
LDI R1, 2   ; Carga 2 en R1
ADD R0, R1   ; Suma: R0 = R0 + R1 (254 + 2 = 256)
OUT R0       ; Envía el resultado (256) al display (Deberia desbordar a 0)

HLT          ; Fin
