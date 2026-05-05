; fibo_16.asm - Fibonacci Multi-Precision (16-Bit) Limits
; A_L = 250, A_H = 251. 
; B_L = 252, B_H = 253.
; Sum_L = 248, Sum_H = 249.
; Limite_H = 255. 181 * 256 + 32 = 46,368. Hit condition is simply A_H == 181.

LDI 0
STA 250    ; A_L = 0
STA 251    ; A_H = 0
STA 253    ; B_H = 0
LDI 1
STA 252    ; B_L = 1
LDI 181    
STA 255    ; Limite_H = 181

; --- INICIO DEL BUCLE (Address 16) ---
LDA 251    ; Display High Byte
OUT
LDA 250    ; Display Low Byte
OUT

; Stop Execution if A_H reaches High Byte target 181 (0xB5)
LDA 251
SUB 255
JZ 72      ; If Zero -> Halts at address 72

; Realizar A_L + B_L
LDA 250
ADD 252
STA 248    ; Guarda temporal Sum_L

; Chequear acarreo en adición inferior
JC 46      ; Branch to Carry Addition Block if Carry==1

; --- BLOQUE: NO CARRY (Address 38) ---
LDA 251
ADD 253
STA 249    ; Sum_H temporal (A_H + B_H) sin acarreo
JMP 54     ; Skip to Shift block at 54

; --- BLOQUE: CARRY (Address 46) ---
LDI 1      ; Sumamos el acarreo previo directamente al RegA
ADD 251
ADD 253
STA 249    ; Sum_H temporal (A_H + B_H + 1)

; --- BLOQUE: DESPLAZAMIENTO (SHIFT) (Address 54) ---
; A = B
LDA 252
STA 250    ; A_L
LDA 253
STA 251    ; A_H
; B = Sum
LDA 248
STA 252    ; B_L
LDA 249
STA 253    ; B_H

JMP 16     ; Repetir infinitamente

; --- DETENCION Y CONTROL (Address 72) ---
HLT
