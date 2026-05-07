; div.asm - Division entera: 10 / 2 = 5
; R0: Dividendo, R1: Divisor, R2: Cociente, R3: Constante 1
; Algoritmo: Resta repetida con deteccion por flag Negative
;
; Mapa de memoria:
;   0: LDI R0 10
;   2: LDI R1 2
;   4: LDI R2 0
;   6: LDI R3 1
;   8: SUB R0 R1    <- LOOP
;  10: JN  18       (si negativo, ir a END)
;  12: ADD R2 R3    (cociente++)
;  14: JZ  18       (si R0 == 0, ir a END)
;  16: JMP 8        (repetir)
;  18: OUT R2       <- END
;  20: HLT

LDI R0 10
LDI R1 2
LDI R2 0
LDI R3 1
LOOP:
SUB R0 R1
JN END      ; Si es negativo, terminamos
ADD R2 R3   ; Incrementar cociente
JZ END      ; Si es exactamente cero, terminamos
JMP LOOP    ; Repetir
 
END:
OUT R2
HLT        