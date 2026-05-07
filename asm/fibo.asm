; fibo_16.asm - Fibonacci al máximo de 16-bit (Signed)
; Este programa calcula la secuencia hasta que desborda el rango de 16 bits.
; Usa la bandera N (Negativo) para detectar cuando la suma supera 32,767.

LDI R0, 0        ; F(n)
LDI R1, 1        ; F(n+1)

LOOP:
OUT R0           ; Mostrar F(n)
LDI R2, 0        ; Preparar R2
NOP              ; Burbuja
ADD R2, R0       ; R2 = R0
NOP              ; Burbuja
ADD R2, R1       ; R2 = R0 + R1 (Siguiente número)
 
; ¿Desbordó los 32,767?
JN  FIN
 
; Actualizar registros para siguiente iteración: R0=R1, R1=R2
LDI R0, 0
NOP
ADD R0, R1
LDI R1, 0
NOP
ADD R1, R2
 
JMP LOOP
 
FIN:
OUT R0           ; Mostrar último válido (28,657)
HLT
