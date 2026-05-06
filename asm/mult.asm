; mult.asm - Multiplicación GPR (3 * 2)
; R0: Contador, R1: Multiplicando, R2: Resultado, R3: 1
LDI R0, 3
LDI R1, 2
LDI R2, 0
LDI R3, 1

; --- LOOP (Addr: 8) ---
ADD R2, R1     ; Resultado += Multiplicando
SUB R0, R3     ; Contador--
JZ  16         ; Si Contador == 0, ir a END
JMP 8          ; Repetir

; --- END (Addr: 16) ---
OUT R2         ; Muestra 6
HLT
