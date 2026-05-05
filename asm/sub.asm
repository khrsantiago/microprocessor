; resta.asm - Resta de 10 - 7
LDI 10   ; Carga el minuendo (10) directamente
STA 200   ; Lo guarda en memoria (opcional, solo para demo)
LDI 7    ; Carga el sustraendo (7)
STA 201   ; Lo guarda en memoria
LDA 200   ; Vuelve a traer el 10 al Acumulador
SUB 201   ; Resta el valor en 201 (10 - 7)
OUT      ; Muestra 3
HLT
