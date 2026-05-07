; blinker.asm - Oscilador "Blinker" de Conway (Juego de la Vida)
; Este programa alterna entre una línea horizontal y una vertical de 3 píxeles.
; Resolución: 16x16, Dirección VRAM inicial: 256.

; --- Inicialización de Constantes ---
LDI R3, 1      ; Constante 1 para incrementos/decrementos
LDI R1, 64
NOP
NOP
ADD R1, R1     ; R1 = 128
NOP
NOP
ADD R1, R1     ; R1 = 256 (Base de la VRAM)
NOP
NOP

; --- Dibujar el Centro Permanente (Fila 8, Col 8 -> Índice 136) ---
; Dirección = Base(256) + 136 = 392
LDI R2, 68     ; 68 * 2 = 136
NOP
NOP
ADD R2, R2     ; R2 = 136
NOP
NOP
ADD R2, R1     ; R2 = 136 + 256 = 392
NOP
NOP
LDI R0, 1      ; Color Blanco
ST [R2], R0    ; Enciende el píxel central permanentemente

MAIN:
    ; === ESTADO 1: HORIZONTAL (Píxeles a los lados del centro) ===
    LDI R0, 1      ; Color Blanco (Encendido)
    
    ; Píxel Izquierdo (392 - 1 = 391)
    LDI R2, 68
    NOP
    NOP
    ADD R2, R2     ; 136
    NOP
    NOP
    SUB R2, R3     ; 135
    NOP
    NOP
    ADD R2, R1     ; 135 + 256 = 391
    NOP
    NOP
    ST [R2], R0
    
    ; Píxel Derecho (392 + 1 = 393)
    LDI R2, 68
    NOP
    NOP
    ADD R2, R2     ; 136
    NOP
    NOP
    ADD R2, R3     ; 137
    NOP
    NOP
    ADD R2, R1     ; 137 + 256 = 393
    NOP
    NOP
    ST [R2], R0
    
    ; --- Limpiar Píxeles del Estado 2 (Vertical) ---
    LDI R0, 0      ; Color Negro (Apagado)
    
    ; Píxel Superior (Y-1 -> 136 - 16 = 120)
    ; Dirección = 120 + 256 = 376
    LDI R2, 120
    NOP
    NOP
    ADD R2, R1     ; 376
    NOP
    NOP
    ST [R2], R0
    
    ; Píxel Inferior (Y+1 -> 136 + 16 = 152)
    ; Dirección = 152 + 256 = 408
    LDI R2, 76     ; 76 * 2 = 152
    NOP
    NOP
    ADD R2, R2     ; 152
    NOP
    NOP
    ADD R2, R1     ; 152 + 256 = 408
    NOP
    NOP
    ST [R2], R0
    
    LDI R0, 1      ; Señal para el visualizador
    OUT R0         ; Actualizar pantalla
    
    ; --- Retardo (Delay) ---
    LDI R2, 20
D1:
    SUB R2, R3
    NOP
    NOP
    JZ D1E
    JMP D1
D1E:

    ; === ESTADO 2: VERTICAL (Píxeles arriba y abajo del centro) ===
    LDI R0, 1      ; Color Blanco (Encendido)
    
    ; Píxel Superior (376)
    LDI R2, 120
    NOP
    NOP
    ADD R2, R1
    NOP
    NOP
    ST [R2], R0
    
    ; Píxel Inferior (408)
    LDI R2, 76
    NOP
    NOP
    ADD R2, R2
    NOP
    NOP
    ADD R2, R1
    NOP
    NOP
    ST [R2], R0
    
    ; --- Limpiar Píxeles del Estado 1 (Horizontal) ---
    LDI R0, 0      ; Color Negro (Apagado)
    
    ; Píxel Izquierdo (391)
    LDI R2, 68
    NOP
    NOP
    ADD R2, R2
    NOP
    NOP
    SUB R2, R3
    NOP
    NOP
    ADD R2, R1
    NOP
    NOP
    ST [R2], R0
    
    ; Píxel Derecho (393)
    LDI R2, 68
    NOP
    NOP
    ADD R2, R2
    NOP
    NOP
    ADD R2, R3
    NOP
    NOP
    ADD R2, R1
    NOP
    NOP
    ST [R2], R0
    
    LDI R0, 1      ; Actualizar pantalla
    OUT R0
    
    ; --- Retardo (Delay) ---
    LDI R2, 20
D2:
    SUB R2, R3
    NOP
    NOP
    JZ D2E
    JMP D2
D2E:
    
    JMP MAIN       ; Volver al inicio del ciclo de animación
