; rain.asm - Una sola gota cayendo por la pantalla.
; Demuestra el cálculo dinámico de direcciones y el uso de memoria RAM para estados.
; RAM [20]: Fila actual (Y), [50]: Base de la VRAM (256)

    ; --- Inicialización ---
    LDI R0, 127
    LDI R1, 1
    ADD R0, R1          ; 128
    NOP
    ADD R0, R0
    ST [50], R0         ; Guardar Base = 256

    LDI R0, 0
    ST [20], R0         ; Y inicial = 0

MAIN:
    LDI R0, 5           ; X fija (columna 5)
    LD R1, [20]         ; R1 = Y actual
    NOP

    ; === 1. BORRAR PIXEL ANTERIOR ===
    LDI R2, 0
    NOP
    ADD R2, R1          ; R2 = Y
    NOP
    ADD R2, R2          ; Y * 2
    NOP
    ADD R2, R2          ; Y * 4
    NOP
    ADD R2, R2          ; Y * 8
    NOP
    ADD R2, R2          ; Y * 16 (Ancho de pantalla)
    NOP
    LD R3, [50]         ; Cargar Base (256)
    NOP
    ADD R2, R3          ; Dirección base de la fila
    NOP
    ADD R2, R0          ; Dirección final = Base + Y*16 + X
    NOP
    LDI R3, 0           ; Color Negro
    NOP
    ST [R2], R3         ; Escribir en VRAM para apagar píxel

    ; === 2. INCREMENTAR POSICIÓN Y ===
    LD R1, [20]         ; Volver a cargar Y
    NOP
    LDI R3, 1
    NOP
    ADD R1, R3          ; Y = Y + 1
    NOP
    LDI R3, 16          ; Límite inferior
    NOP
    SUB R3, R1          ; ¿Llegamos al final (16)?
    JZ WRAP
    JMP SAVE_Y
WRAP:
    LDI R1, 0           ; Reiniciar Y a arriba si llegamos al final
SAVE_Y:
    ST [20], R1         ; Guardar nueva Y en RAM

    ; === 3. PINTAR NUEVO PIXEL ===
    LDI R3, 1           ; Color blanco (RGB básico)
    NOP
    LDI R2, 0
    ADD R2, R1          ; R2 = Nueva Y
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2          ; Nueva Y * 16
    NOP
    LD R1, [50]         ; Base VRAM
    NOP
    ADD R2, R1          ; Dirección base de nueva fila
    NOP
    ADD R2, R0          ; + columna X
    NOP
    ST [R2], R3         ; Encender píxel en nueva posición

    OUT R0              ; Actualizar pantalla para mostrar el movimiento
    JMP MAIN            ; Bucle infinito
