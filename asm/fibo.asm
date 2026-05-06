; fibo.asm - Secuencia de Fibonacci hasta 233
; R0: fib(n), R1: fib(n+1), R2: temporal, R3: limite (233)
;
; Nota: Se incluyen NOPs para evitar riesgos de datos (Hazards) en el pipeline
; ya que algunas instrucciones consecutivas dependen del resultado anterior.

LDI R0 0
LDI R1 1
LDI R3 233

; --- LOOP (Addr: 6) ---
OUT R0
LDI R2 0         ; Usar R2 para comparar R0 con el límite sin destruir R0
NOP
ADD R2 R0        ; R2 = R0
SUB R2 R3        ; R2 = R0 - 233
NOP              ; Bubble para flags
JZ 44            ; Si R0 era 233, terminar (HLT en Addr 44)

LDI R2 0         ; Reiniciar R2 para el cálculo de la suma
NOP
ADD R2 R0
NOP
ADD R2 R1        ; R2 = R0 + R1 (siguiente)

LDI R0 0
NOP
ADD R0 R1        ; R0 = viejo R1

LDI R1 0
NOP
ADD R1 R2        ; R1 = nuevo fib
JMP 6            ; Repetir loop (Addr 6)

HLT              ; Addr 44
