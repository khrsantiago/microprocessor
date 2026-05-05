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

; Dir 0: Muestra el patron actual
OUT
; Dir 2: Ahora invertimos: leemos cada celda, la intercambiamos

; ======= FRAME A -> FRAME B (horizontal -> vertical) =======
; Borramos fila 1 (237,238,239) y pintamos columna 1 (233,238,243)
; Pero para hacerlo genéricamente con RegX, simplemente
; recalculamos las 3 celdas de la fila y las 3 de la columna

; Borrar 237
LDI 0
STA 237
; Dir 6
STA 239
; Dir 8: Pintar columna central (233, 238, 243)
LDI 1
; Dir 10
STA 233
; Dir 12: 238 ya tiene 1 (centro)
STA 243
; Dir 14
OUT
; Dir 16: Pausa (NOP ayuda a espaciar los OUTs)
NOP

; ======= FRAME B -> FRAME A (vertical -> horizontal) =======
; Dir 18: Borrar columna
LDI 0
; Dir 20
STA 233
; Dir 22
STA 243
; Dir 24: Pintar fila
LDI 1
; Dir 26
STA 237
; Dir 28
STA 239
; Dir 30
OUT
; Dir 32
NOP

; Dir 34: Loop infinito
JMP 2
