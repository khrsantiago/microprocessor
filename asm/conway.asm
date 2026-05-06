; conway.asm - Conway's Game of Life (KHR-8 GPR Labels Version)
; R0: Acc / Data, R1: Index / Addr, R2: Temp, R3: Flags

; ============ CONSTANTES ============
150:
CELL_PTR: 0
151:
OFF_PTR:  0
152:
COUNT:    0
153:
TEMP:     0

160:
CONST_1:  1
161:
CONST_2:  2
162:
CONST_3:  3
163:
CONST_25: 25

; Offsets de vecinos (170-178)
170:
OFFSETS:  250 ; -6
171: 251 ; -5
172: 252 ; -4
173: 255 ; -1
174: 1   ; +1
175: 4   ; +4
176: 5   ; +5
177: 6   ; +6
178: 0   ; FIN

; Patron inicial (Blinker)
237: 1
238: 1
239: 1

; ============ CODIGO ============
; === MAIN_LOOP ===
0:
MAIN_LOOP:
    LDI R0, 231
    STA CELL_PTR, R0

CELL_LOOP:
    LDI R0, 0
    STA COUNT, R0
    LDI R0, OFFSETS
    STA OFF_PTR, R0

N_LOOP:
    LD R1, [OFF_PTR]
    LD R0, [R1]
    JZ N_END            ; Si offset == 0, fin de vecinos
    
    STA TEMP, R0
    LD R0, [CELL_PTR]
    LD R2, [TEMP]
    ADD R0, R2          ; A = cell_ptr + offset
    MOV R1, R0
    NOP                 ; Safety for indirect load
    LD R0, [R1]         ; Cargar vecino
    LD R2, [COUNT]
    ADD R0, R2
    STA COUNT, R0
    
    LD R0, [OFF_PTR]
    LD R2, [CONST_1]
    ADD R0, R2
    STA OFF_PTR, R0
    JMP N_LOOP

N_END:
    LD R0, [COUNT]
    LD R2, [CONST_2]
    SUB R0, R2          ; A = Count - 2
    JC CHK_BORN         ; Si A >= 0 (Count >= 2)
    JMP DIE             ; Si Count < 2

CHK_BORN:
    LD R0, [COUNT]
    LD R2, [CONST_3]
    SUB R0, R2          ; A = Count - 3
    JZ BORN             ; Si Count == 3
    JC DIE              ; Si Count > 3

SURVIVE:
    LD R1, [CELL_PTR]
    LD R0, [R1]
    JMP WRITE

BORN:
    LDI R0, 1
    JMP WRITE

DIE:
    LDI R0, 0

WRITE:
    STA TEMP, R0        ; Guardar resultado nuevo
    LD R0, [CELL_PTR]
    LD R2, [CONST_25]
    SUB R0, R2          ; A = CELL_PTR - 25 (Parallel Buffer)
    MOV R1, R0
    LD R0, [TEMP]
    ST [R1], R0         ; Escribir al buffer

ADVANCE:
    LD R0, [CELL_PTR]
    LD R2, [CONST_1]
    ADD R0, R2
    STA CELL_PTR, R0
    JZ END_FRAME        ; Si overflow (0), fin de grid
    JMP CELL_LOOP

END_FRAME:
    LDI R0, 206         ; Buffer start
    STA OFF_PTR, R0
    LDI R0, 231         ; VRAM start
    STA CELL_PTR, R0

COPY_LOOP:
    LD R1, [OFF_PTR]
    LD R0, [R1]
    STA TEMP, R0
    LD R1, [CELL_PTR]
    LD R0, [TEMP]
    ST [R1], R0
    
    LD R0, [OFF_PTR]
    LD R2, [CONST_1]
    ADD R0, R2
    STA OFF_PTR, R0
    LD R0, [CELL_PTR]
    LD R2, [CONST_1]
    ADD R0, R2
    STA CELL_PTR, R0
    JZ FLUSH
    JMP COPY_LOOP

FLUSH:
    OUT R0
    JMP MAIN_LOOP
