; scanner.asm - Escáner lineal de VRAM (Prueba de hardware)
; Recorre secuencialmente todas las posiciones de la VRAM (256-511).

; --- 1. Inicializar Puntero de memoria a 256 (Inicio de VRAM) ---
LDI R0, 127
LDI R3, 1
ADD R0, R3     ; R0 = 128
NOP
ADD R0, R0     ; R0 = 256
NOP

; --- 2. Establecer Límite de exclusión en 512 (Fin de VRAM + 1) ---
LDI R2, 127
LDI R3, 1
ADD R2, R3     ; R2 = 128
NOP
ADD R2, R2     ; R2 = 256
NOP
ADD R2, R2     ; R2 = 512
NOP

LDI R3, 1      ; Constante para incremento unitario

LOOP:
; --- Acceso a VRAM ---
LDI R1, 1      ; Color Blanco (Encendido)
ST [R0], R1    ; Enciende el píxel apuntado por R0

; Renderizar en el visualizador
OUT R0

; Limpiar el píxel inmediatamente para crear el efecto de "punto en movimiento"
LDI R1, 0      ; Color Negro (Apagado)
ST [R0], R1

; --- Avanzar al siguiente píxel ---
ADD R0, R3

; --- Comprobación de Fin de VRAM ---
LDI R1, 0
ADD R1, R0     ; Copiar puntero actual a R1 para comparar
SUB R1, R2     ; ¿Puntero actual == 512?
JZ FIN         ; Si es igual, terminar
JMP LOOP       ; Si no, seguir con el siguiente píxel

FIN:
HLT            ; Detener ejecución
