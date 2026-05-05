; fibo.asm - Secuencia de Fibonacci hasta 233
; 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233
; Algoritmo:
; A = 0, B = 1
; Límite = 233

LDI 0
STA 250    ; Mem[250] es 'A' (anterior)
LDI 1
STA 251    ; Mem[251] es 'B' (actual)

LDI 233
STA 253    ; Mem[253] es el Limite (233)

; --- INICIO DEL BUCLE (Dirección 12) ---
LDA 250    ; Carga A
OUT        ; Muestra el número actual de la secuencia

; Verificar si el valor recién impreso es 233
LDA 250
SUB 253    ; Si A == 233, entonces A - 233 = 0, activa Zero Flag
JZ 38      ; Si es 0, saltar a HLT (Direccion 38)

LDA 250    ; Volver a cargar A
ADD 251    ; Suma A + B
STA 252    ; Guarda temporalmente la suma en Mem[252]

LDA 251    ; Trae B
STA 250    ; Ahora B es el nuevo A (desplazamiento)

LDA 252    ; Trae la Suma
STA 251    ; Ahora la Suma es el nuevo B

JMP 12     ; Repetir infinitamente

HLT        ; Detener la ejecución (Dirección 38)
