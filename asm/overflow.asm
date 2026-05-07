; overflow_test_16bit.asm - Verificación de bandera de Overflow en 16 bits
; En 16 bits, el límite positivo es 32767 (0x7FFF). 
; 32767 + 1 deberia activar el Flag O (Overflow) y dar -32768 (0x8000).

; Construyendo 32767 en R0
LDI R0, 127
LDI R1, 127
ADD R0, R1      ; R0 = 254
ADD R0, R0      ; R0 = 508
ADD R0, R0      ; R0 = 1016
ADD R0, R0      ; R0 = 2032
ADD R0, R0      ; R0 = 4064
ADD R0, R0      ; R0 = 8128
ADD R0, R0      ; R0 = 16256
ADD R0, R0      ; R0 = 32512
LDI R1, 255
ADD R0, R1      ; R0 = 32512 + 255 = 32767 (0x7FFF)

OUT R0          ; Mostrar 32767

LDI R1, 1
ADD R0, R1      ; 32767 + 1 = 32768 (Overflow! -> -32768)

OUT R0          ; Deberia mostrar -32768
HLT
