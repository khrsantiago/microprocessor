; scanner_dedicated.asm - Prueba robusta de VRAM Dedicada (0x0100)
; R0: Puntero VRAM, R1: Valor, R2: Limite (512)
 
; 1. Inicializar Puntero VRAM a 256
LDI R0, 127
LDI R3, 1
ADD R0, R3     ; R0 = 128
NOP
ADD R0, R0     ; R0 = 256 (Inicio VRAM)
NOP
 
; 2. Inicializar Limite a 512
LDI R2, 127
LDI R3, 1
ADD R2, R3     ; R2 = 128
NOP
ADD R2, R2     ; R2 = 256
NOP
ADD R2, R2     ; R2 = 512 (Limite exclusión)
NOP
 
LDI R3, 1      ; Incremento constante
 
LOOP:
; Encender píxel en VRAM (Puntero en R0 >= 256)
LDI R1, 1
ST [R0], R1
 
; Renderizar (Actualizar pantalla)
OUT R0
 
; Apagar píxel anterior
LDI R1, 0
ST [R0], R1
 
; Siguiente píxel
ADD R0, R3
 
; Comparar con 512
LDI R1, 0
ADD R1, R0
SUB R1, R2
JZ FIN
 
JMP LOOP
 
FIN:
HLT
