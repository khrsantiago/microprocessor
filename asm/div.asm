; div.asm - División de 10 / 2 (Software)
; Algoritmo: Restas sucesivas (Exacta)
; 0: LDI 10
; 1: STA 13
; 2: LDI 2
; 3: STA 14
; 4: LDI 0
; 5: STA 15
; 6: LDA 13 (LOOP)
; 7: SUB 14
; 8: JZ 14  (saltar a ZERO)
; 9: STA 13 (nuevo resto)
; 10: LDA 15 (inc cociente)
; 11: ADD 12 (RAM[12]=1)
; 12: STA 15
; 13: JMP 6
; 14: LDA 15 (ZERO)
; 15: ADD 12
; 16: OUT
; 17: HLT

LDI 10
STA 250
LDI 2
STA 251
LDI 0
STA 252

; --- LOOP --- (Address 12)
LDA 250  
SUB 251
JZ 28      ; Salto a ZERO si da 0

STA 250
LDA 252
ADD 249
STA 252
JMP 12     ; Vuelve a LOOP

; --- ZERO --- (Address 28)
LDA 252   
ADD 249
OUT
HLT

249: 1
