; checker.asm - Patrón de Tablero de Ajedrez Animado
; Dibuja un tablero de 16x16 que alterna colores entre celdas.

; --- Mapa de RAM ---
; [10]: Base VRAM (256), [11]: Color 1, [12]: Color 2
; [2]: Contador Y, [3]: Contador X, [4]: Selector inicio fila, [5]: Selector celda actual

INIT:
    ; Calcular e inicializar Base VRAM (256)
    LDI R0, 127
    LDI R1, 1
    ADD R0, R1          ; 128
    NOP
    ADD R0, R0          ; 256
    ST [10], R0

    ; Inicializar colores (Formato RGB565)
    LDI R0, 0           ; Color 1: Negro
    ST [11], R0
    LDI R0, 31          ; Color 2: Azul puro (0x001F)
    ST [12], R0

    ; RAM[4] guarda si la fila actual empieza con Color 1 o Color 2
    LDI R0, 0
    ST [4], R0

ANIM:
    ; Inicio de frame
    LDI R0, 0           ; Y = 0
    ST [2], R0

LOOP_Y:
    ; Al iniciar cada fila, cargamos el selector de color inicial de esa fila
    LD R1, [4]          ; R1 = selector (0 o 1)
    NOP
    LDI R0, 0           ; X = 0
    ST [3], R0

LOOP_X:
    ST [5], R1          ; Guardar selector de la celda actual

    ; --- Calcular Dirección VRAM: Base + Y*16 + X ---
    LD R0, [2]          ; Cargar Y
    NOP
    LDI R2, 0
    ADD R2, R0          ; R2 = Y
    NOP
    ADD R2, R2          ; Y * 2
    NOP
    ADD R2, R2          ; Y * 4
    NOP
    ADD R2, R2          ; Y * 8
    NOP
    ADD R2, R2          ; Y * 16
    NOP
    LD R3, [10]         ; Cargar Base (256)
    NOP
    ADD R2, R3          ; Base + (Y * 16)
    NOP
    LD R3, [3]          ; Cargar X
    NOP
    ADD R2, R3          ; R2 = Dirección Final en VRAM

    ; --- Seleccionar Color según el valor de R1 ---
    LD R1, [5]          ; Recuperar selector
    NOP
    LDI R3, 0
    SUB R1, R3          ; ¿Selector es 0?
    JZ USE_C1
    LD R3, [12]         ; Si no es 0, usar Color 2
    JMP PAINT
USE_C1:
    LD R3, [11]         ; Si es 0, usar Color 1
PAINT:
    ST [R2], R3         ; Dibujar píxel en VRAM

    ; --- Alternar Selector para la siguiente celda (X) ---
    LD R1, [5]
    NOP
    LDI R3, 0
    SUB R1, R3
    JZ TO_1
    LDI R1, 0           ; Si era 1, pasar a 0
    JMP NEXT_X
TO_1:
    LDI R1, 1           ; Si era 0, pasar a 1

NEXT_X:
    LD R0, [3]          ; Cargar X
    NOP
    LDI R3, 1
    ADD R0, R3          ; X++
    NOP
    ST [3], R0
    
    ; ¿Completamos la fila (X == 16)?
    LDI R3, 16
    SUB R3, R0
    JZ ADVANCE_Y
    JMP LOOP_X          ; Siguiente celda de la fila

ADVANCE_Y:
    ; --- Alternar Selector inicial para la siguiente fila (Y) ---
    LD R1, [4]
    NOP
    LDI R3, 0
    SUB R1, R3
    JZ ROW_TO_1
    LDI R1, 0
    JMP SAVE_ROW
ROW_TO_1:
    LDI R1, 1
SAVE_ROW:
    ST [4], R1          ; Guardar nuevo color inicial de fila

    ; --- Incrementar Y ---
    LD R0, [2]
    NOP
    LDI R3, 1
    ADD R0, R3
    NOP
    ST [2], R0
    
    ; ¿Completamos todas las filas (Y == 16)?
    LDI R3, 16
    SUB R3, R0
    JZ DRAW_FRAME
    JMP LOOP_Y          ; Siguiente fila

DRAW_FRAME:
    OUT R0              ; Actualizar pantalla con el tablero completo

    ; --- Intercambiar los colores para el siguiente frame (Animación) ---
    LD R0, [11]
    LD R1, [12]
    ST [11], R1         ; C1 = C2
    ST [12], R0         ; C2 = C1
    JMP ANIM            ; Volver a dibujar todo el tablero invertido
