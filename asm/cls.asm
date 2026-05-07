; cls.asm - Limpiar la pantalla de 16x16 (256 píxeles)
; R0: Puntero de VRAM, R1: Color (0 = Negro), R3: Incremento(1), R2: Límite
 
; Inicialización
LDI R0, 0      ; Empezar en RAM[0]
LDI R1, 0      ; Color negro (RGB565 0x0000)
LDI R3, 1      ; Paso = 1
 
; Construir el límite 256 en R2 (para comparar)
LDI R2, 127
LDI R3, 1
ADD R2, R3     ; R2 = 128
ADD R2, R2     ; R2 = 256
 
LDI R3, 1      ; Restaurar R3 = 1
 
LOOP:
ST [R0], R1    ; RAM[R0] = 0 (Negro)
ADD R0, R3     ; R0++
 
; Ver si R0 == R2 (256)
LDI R1, 0      ; Usar R1 temporalmente (el color ya está en memoria)
ADD R1, R0
SUB R1, R2     ; R1 = R0 - 256
 
JZ FIN         ; Si R0 == 256, terminar
 
LDI R1, 0      ; Restaurar R1 = 0 (color negro) para el siguiente loop
JMP LOOP
 
FIN:
OUT R0         ; Un último OUT para refrescar el frame final
HLT
