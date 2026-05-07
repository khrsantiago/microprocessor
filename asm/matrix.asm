; matrix.asm - Simulación del efecto "Matrix rain" (Lluvia Digital)
; Gestiona 4 gotas independientes de forma secuencial y segura para el pipeline.
; RAM: [20-23]: Coordenadas Y, [24-27]: Coordenadas X, [50]: Base VRAM, [51]: Color Verde

INIT:
    ; --- 1. Calcular Base VRAM (256) ---
    LDI R0, 127
    LDI R1, 1
    ADD R0, R1          ; 128
    NOP
    ADD R0, R0          ; 256
    ST [50], R0         ; Guardar en RAM[50]
    
    ; --- 2. Definir Color Verde Matrix (RGB565) ---
    ; Queremos un verde brillante. 0x07E0 es Verde puro en RGB565.
    LDI R0, 63          ; Valor de bits para el componente verde
    NOP
    ADD R0, R0          ; Desplazamientos para alinear con el formato RGB565
    NOP
    ADD R0, R0
    NOP
    ADD R0, R0
    NOP
    ADD R0, R0
    NOP
    ADD R0, R0
    ST [51], R0         ; Guardar color en RAM[51]
    
    ; --- 3. Inicializar Posiciones de las 4 Gotas ---
    ; Gota 1: (X=1, Y=0)
    LDI R0, 0
    ST [20], R0 
    LDI R0, 1
    ST [24], R0 
    ; Gota 2: (X=5, Y=4)
    LDI R0, 4
    ST [21], R0 
    LDI R0, 5
    ST [25], R0 
    ; Gota 3: (X=10, Y=8)
    LDI R0, 8
    ST [22], R0 
    LDI R0, 10
    ST [26], R0 
    ; Gota 4: (X=14, Y=12)
    LDI R0, 12
    ST [23], R0 
    LDI R0, 14
    ST [27], R0 

MAIN:
    ; --- PROCESAR GOTA 1 ---
    LD R0, [24]        ; Cargar X1
    LD R1, [20]        ; Cargar Y1
    NOP
    NOP
    NOP                ; Burbujas para evitar conflictos de lectura

    ; Borrar posición anterior
    LDI R2, 0
    ADD R2, R1
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2          ; Y1 * 16
    LD R3, [50]
    NOP
    NOP
    ADD R2, R3          ; Base + Y1*16
    ADD R2, R0          ; + X1
    LDI R3, 0           ; Color Negro
    ST [R2], R3

    ; Mover gota hacia abajo
    LDI R3, 1
    ADD R1, R3          ; Y1 = Y1 + 1
    NOP
    LDI R3, 16          ; Límite de pantalla
    SUB R3, R1
    JZ R1_T             ; Si llegó a 16, volver a arriba
    JMP R1_S
R1_T: LDI R1, 0         ; Reset Y a 0
R1_S: ST [20], R1       ; Guardar nueva Y1

    ; Pintar nueva posición
    LDI R2, 0
    ADD R2, R1
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2          ; Y * 16
    LD R3, [50]
    NOP
    ADD R2, R3
    ADD R2, R0
    LD R3, [51]         ; Cargar Color Verde
    NOP
    ST [R2], R3

    ; --- PROCESAR GOTA 2 ---
    ; (Misma lógica: Borrar, Mover, Pintar)
    LD R0, [25]
    LD R1, [21]
    NOP
    NOP
    NOP
    LDI R2, 0
    ADD R2, R1
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    LD R3, [50]
    NOP
    ADD R2, R3
    ADD R2, R0
    LDI R3, 0
    ST [R2], R3
    LDI R3, 1
    ADD R1, R3
    NOP
    LDI R3, 16
    SUB R3, R1
    JZ R2_T
    JMP R2_S
R2_T: LDI R1, 0
R2_S: ST [21], R1
    LDI R2, 0
    ADD R2, R1
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    LD R3, [50]
    NOP
    ADD R2, R3
    ADD R2, R0
    LD R3, [51]
    NOP
    ST [R2], R3

    ; --- PROCESAR GOTA 3 ---
    LD R0, [26]
    LD R1, [22]
    NOP
    NOP
    NOP
    LDI R2, 0
    ADD R2, R1
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    LD R3, [50]
    NOP
    ADD R2, R3
    ADD R2, R0
    LDI R3, 0
    ST [R2], R3
    LDI R3, 1
    ADD R1, R3
    NOP
    LDI R3, 16
    SUB R3, R1
    JZ R3_T
    JMP R3_S
R3_T: LDI R1, 0
R3_S: ST [22], R1
    LDI R2, 0
    ADD R2, R1
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    LD R3, [50]
    NOP
    ADD R2, R3
    ADD R2, R0
    LD R3, [51]
    NOP
    ST [R2], R3

    ; --- PROCESAR GOTA 4 ---
    LD R0, [27]
    LD R1, [23]
    NOP
    NOP
    NOP
    LDI R2, 0
    ADD R2, R1
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    LD R3, [50]
    NOP
    ADD R2, R3
    ADD R2, R0
    LDI R3, 0
    ST [R2], R3
    LDI R3, 1
    ADD R1, R3
    NOP
    LDI R3, 16
    SUB R3, R1
    JZ R4_T
    JMP R4_S
R4_T: LDI R1, 0
R4_S: ST [23], R1
    LDI R2, 0
    ADD R2, R1
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    LD R3, [50]
    NOP
    ADD R2, R3
    ADD R2, R0
    LD R3, [51]
    NOP
    ST [R2], R3

    OUT R0              ; Actualizar pantalla después de mover las 4 gotas
    JMP MAIN            ; Repetir bucle infinito
