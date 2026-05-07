; sqrt.asm - GPR Modernized Version
; Algoritmo: Raíz Cuadrada Entera (Método de restas de impares)
; R0: N (Valor a reducir)
; R1: Odd (1, 3, 5, 7...)
; R2: Root (Contador del resultado)
; R3: Temporal / Constantes

; --- Inicialización ---
; --- Inicialización ---
LDI R0, 112   ; N parte 1
LDI R1, 113   ; N parte 2 (112 + 113 = 225)
ADD R0, R1    ; R0 ahora es 225 (sin problemas de signo)
LDI R1, 1     ; Odd = 1
LDI R2, 0     ; Root = 0
LDI R3, 2     ; Constante incrementador (para Odd += 2)
LDI R3, 2     ; (Se repite para claridad de puertos)

LOOP:
SUB R0, R1    ; R0 = R0 - R1
JZ  FOUND     ; Raiz perfecta -> FOUND
JC  CONTINUE  ; Seguir restando -> CONTINUE
JMP END       ; Terminar -> END
 
CONTINUE:
LDI R3, 1
ADD R2, R3    ; Root++
OUT R2        ; Mostrar progreso
LDI R3, 2
ADD R1, R3    ; Odd += 2
JMP LOOP      ; Repeat loop
 
FOUND:
LDI R3, 1
ADD R2, R3    ; Contar ultima iteración
 
END:
OUT R2        ; Mostrar Raiz Final
HLT
