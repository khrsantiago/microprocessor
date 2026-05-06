; KHR-8 GPR Architecture Test
; Este programa suma 10 + 7 usando registros y verifica la memoria
LDI R1, 10      ; R1 = 10
LDI R2, 7       ; R2 = 7
ADD R1, R2      ; R1 = R1 + R2 = 17
ST 15, R1       ; Guardar 17 en RAM[15]
LDI R1, 0       ; Limpiar R1
LD R1, 15       ; Cargar desde RAM[15] (R1 debe ser 17)
OUT R1          ; Mostrar 17
HLT             ; Fin del programa
