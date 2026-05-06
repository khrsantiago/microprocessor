; blinker.asm - Animación Blinker simple para probar VRAM
; Alterna entre patrón horizontal y vertical usando RegX
; VRAM 5x5: Direcciones 231-255
;
; Precarga de datos
160: 1
231: 0
232: 0
233: 0
234: 0
235: 0
236: 0
237: 1
238: 1
239: 1
240: 0
241: 0
242: 0
243: 0
244: 0
245: 0
246: 0
247: 0
248: 0
249: 0
250: 0
251: 0
252: 0
253: 0
254: 0
255: 0

0:
OUT R0

LOOP:
; ======= FRAME A -> FRAME B (horizontal -> vertical) =======
; Borrar 237
LDI R0, 0
STA 237, R0
STA 239, R0

; Pintar columna central (233, 238, 243)
LDI R0, 1
STA 233, R0
STA 243, R0

NOP
NOP
OUT R0
NOP

; ======= FRAME B -> FRAME A (vertical -> horizontal) =======
; Borrar columna
LDI R0, 0
STA 233, R0
STA 243, R0

; Pintar fila
LDI R0, 1
STA 237, R0
STA 239, R0

NOP
NOP
OUT R0
NOP

; Loop infinito
JMP LOOP
