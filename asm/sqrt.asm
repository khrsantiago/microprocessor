; sqrt.asm - GPR Modernized Version
; Algoritmo: Raíz Cuadrada Entera (Método de restas de impares)
; R0: N (Valor a reducir)
; R1: Odd (1, 3, 5, 7...)
; R2: Root (Contador del resultado)
; R3: Temporal / Constantes

; --- Inicialización ---
LDI R0, 225   ; N = 225
LDI R1, 1     ; Odd = 1
LDI R2, 0     ; Root = 0
LDI R3, 2     ; Constante incrementador

; --- Main Loop (Addr: 8) ---
SUB R0, R1    ; R0 = R0 - R1
JZ  28        ; Raiz perfecta -> FOUND
JC  16        ; Seguir restando -> CONTINUE
JMP 32        ; Terminar -> END

; --- CONTINUE (Addr: 16) ---
LDI R3, 1
ADD R2, R3    ; Root++
OUT R2        ; Mostrar progreso
LDI R3, 2
ADD R1, R3    ; Odd += 2
JMP 8         ; Repeat loop

; --- FOUND (Addr: 28) ---
LDI R3, 1
ADD R2, R3    ; Contar ultima iteración

; --- END (Addr: 32) ---
OUT R2        ; Mostrar Raiz Final
HLT
