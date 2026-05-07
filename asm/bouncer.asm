; bouncer.asm - Pelota rebotando en pantalla de 16x16
; Simula el movimiento de un píxel que rebota en los bordes y cambia de color.

; --- Mapa de Memoria RAM ---
; [10]: X actual, [11]: Y actual, [12]: DX (Dirección X), [13]: DY (Dirección Y)
; [14]: Color actual (RGB565), [15]: Base de VRAM (256)

INIT:
    LDI R0, 0
    ST [10], R0        ; X inicial = 0
    ST [11], R0        ; Y inicial = 0
    LDI R0, 1
    ST [12], R0        ; DX = 1 (derecha)
    ST [13], R0        ; DY = 1 (abajo)
    LDI R0, 1          ; Color inicial = Blanco (basico)
    ST [14], R0
    
    ; Calcular Constante 256 (Base de VRAM)
    LDI R0, 127
    LDI R3, 1
    ADD R0, R3         ; R0 = 128
    NOP
    ADD R0, R0         ; R0 = 256
    ST [15], R0        ; Guardar Base en RAM[15]

MAIN:
    ; --- 1. Borrar píxel en la posición anterior ---
    LD R1, [11]         ; Cargar Y
    NOP
    ADD R1, R1          ; Y * 2
    NOP
    ADD R1, R1          ; Y * 4
    NOP
    ADD R1, R1          ; Y * 8
    NOP
    ADD R1, R1          ; Y * 16 (Desplazamiento de fila)
    LD R3, [15]         ; Cargar Base (256)
    NOP
    ADD R1, R3          ; Base + (Y * 16)
    LD R3, [10]         ; Cargar X
    NOP
    ADD R1, R3          ; Dirección final = Base + Y*16 + X
    LDI R0, 0           ; Color Negro
    ST [R1], R0         ; Escribir en VRAM para borrar

    ; --- 2. Actualizar posición X ---
    LD R0, [10]        ; X actual
    LD R1, [12]        ; Velocidad DX
    NOP
    ADD R0, R1         ; X = X + DX
    ST [10], R0        ; Guardar nueva X
    
    ; --- Rebote en bordes de X (0 y 15) ---
    JZ CHG_DX          ; Si X == 0, rebotar
    LDI R2, 15
    SUB R2, R0         ; 15 - X
    JZ CHG_DX          ; Si X == 15, rebotar
    JMP MOVE_Y         ; Si no llega a borde, seguir con Y

CHG_DX:
    LD R0, [12]        ; Cargar DX actual
    LDI R1, 0
    NOP
    SUB R1, R0         ; DX = 0 - DX (Invertir dirección)
    ST [12], R1
    
    ; Cambiar color al rebotar
    LD R0, [14]        ; Cargar color actual
    LDI R1, 20         ; Incrementar valor cromático
    NOP
    ADD R0, R1
    ST [14], R0

MOVE_Y:
    ; --- 3. Actualizar posición Y ---
    LD R0, [11]
    LD R1, [13]
    NOP
    ADD R0, R1
    ST [11], R0
    
    ; --- Rebote en bordes de Y (0 y 15) ---
    JZ CHG_DY          ; Si Y == 0, rebotar
    LDI R2, 15
    SUB R2, R0         ; 15 - Y
    JZ CHG_DY          ; Si Y == 15, rebotar
    JMP DRAW           ; Ir a dibujar

CHG_DY:
    LD R0, [13]
    LDI R1, 0
    NOP
    SUB R1, R0         ; DY = 0 - DY (Invertir dirección)
    ST [13], R1

DRAW:
    ; --- 4. Pintar píxel en la nueva posición ---
    LD R1, [11]         ; Cargar Y
    NOP
    ADD R1, R1
    NOP
    ADD R1, R1
    NOP
    ADD R1, R1
    NOP
    ADD R1, R1          ; Y * 16
    LD R3, [15]         ; Base VRAM
    NOP
    ADD R1, R3
    LD R3, [10]         ; Cargar X
    NOP
    ADD R1, R3          ; R1 = Dirección de destino
    LD R0, [14]         ; Cargar Color actual
    NOP
    ST [R1], R0         ; Escribir en VRAM
    OUT R0              ; Actualizar visualizador

    ; --- 5. Retardo (Delay) para controlar velocidad ---
    LDI R2, 50
DLY:
    LDI R3, 1
    SUB R2, R3
    JZ MAIN             ; Cuando el contador llega a 0, volver al MAIN
    JMP DLY
