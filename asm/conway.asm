; conway.asm - Conway's Game of Life (KHR-8, 8-Bit)
;
; MEMORY MAP:
;   0-119   : Código (instrucciones del programa)
;   150     : CELL_PTR
;   151     : OFF_PTR
;   152     : COUNT
;   153     : TEMP
;   160     : CONST_1  = 1  (precargada por ensamblador)
;   161     : CONST_2  = 2
;   162     : CONST_3  = 3
;   163     : CONST_25 = 25
;   170-178 : Offsets de vecinos (precargados)
;   206-230 : Buffer paralelo (25 celdas)
;   231-255 : VRAM 5x5 (visible en pantalla)

; ============ DATOS PRECARGADOS (Directivas de direccion) ============
160: 1
161: 2
162: 3
163: 25
170: 250
171: 251
172: 252
173: 255
174: 1
175: 4
176: 5
177: 6
178: 0
237: 1
238: 1
239: 1

; ============ CODIGO (Dir 0) ============
; === MAIN_LOOP (Dir 0): Inicia generacion ===
; Dir 0
LDI 231
; Dir 2
STA 150

; === CELL_LOOP (Dir 4) ===
; Dir 4
LDI 0
; Dir 6
STA 152
; Dir 8
LDI 170
; Dir 10
STA 151

; === N_LOOP (Dir 12): Itera sobre vecinos ===
; Dir 12
LDA 151
; Dir 14
TAX
; Dir 16
LDA_X
; Dir 18: Si offset==0, fin de vecinos -> N_END (Dir 46)
JZ 46

; Dir 20
STA 153
; Dir 22
LDA 150
; Dir 24
ADD 153
; Dir 26
TAX
; Dir 28
LDA_X
; Dir 30
ADD 152
; Dir 32
STA 152

; Dir 34
LDA 151
; Dir 36
ADD 160
; Dir 38
STA 151
; Dir 40
JMP 12
; Dir 42
NOP
; Dir 44
NOP

; === N_END (Dir 46): Reglas de Conway ===
; Dir 46
LDA 152
; Dir 48
SUB 161
; Dir 50: Count >= 2 -> CHK3 (Dir 56)
JC 56
; Dir 52: Count < 2 -> DIE (Dir 76)
JMP 76
; Dir 54
NOP

; === CHK3 (Dir 56) ===
; Dir 56
LDA 152
; Dir 58
SUB 162
; Dir 60: Count == 3 -> BORN (Dir 72)
JZ 72
; Dir 62: Count > 3 -> DIE (Dir 76)
JC 76

; === SURVIVE (Dir 64) ===
; Dir 64
LDA 150
; Dir 66
TAX
; Dir 68
LDA_X
; Dir 70
JMP 80

; === BORN (Dir 72) ===
; Dir 72
LDI 1
; Dir 74
JMP 80

; === DIE (Dir 76) ===
; Dir 76
LDI 0
; Dir 78
NOP

; === WRITE (Dir 80) ===
; Dir 80
STA 153
; Dir 82
LDA 150
; Dir 84
SUB 163
; Dir 86
TAX
; Dir 88
LDA 153
; Dir 90
STA_X

; === ADVANCE (Dir 92) ===
; Dir 92
LDA 150
; Dir 94
ADD 160
; Dir 96
STA 150
; Dir 98: Si overflow (255+1=0) -> END_FRAME (Dir 104)
JZ 104
; Dir 100
JMP 4
; Dir 102
NOP

; === END_FRAME (Dir 104): Copia buffer a VRAM ===
; Dir 104
LDI 206
; Dir 106
STA 151
; Dir 108
LDI 231
; Dir 110
STA 150

; === COPY_LOOP (Dir 112) ===
; Dir 112
LDA 151
; Dir 114
TAX
; Dir 116
LDA_X
; Dir 118
STA 153

; Dir 120 (FIXME: need to continue but running out of room!)
; Actually wait, let me count: Dir 120 is fine, we have until 149.

; Dir 120
LDA 150
; Dir 122
TAX
; Dir 124
LDA 153
; Dir 126
STA_X

; Dir 128
LDA 151
; Dir 130
ADD 160
; Dir 132
STA 151
; Dir 134
LDA 150
; Dir 136
ADD 160
; Dir 138
STA 150
; Dir 140: Si overflow -> FLUSH (Dir 146)
JZ 146
; Dir 142
JMP 112
; Dir 144
NOP

; === FLUSH (Dir 146) ===
; Dir 146
OUT
; Dir 148
JMP 0
