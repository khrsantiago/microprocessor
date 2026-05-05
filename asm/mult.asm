; mult.asm - Multiplicación de 3 * 2 (Software)
; Algoritmo: Sumas sucesivas
; Direcciones:
; 0: LDI 3
; 1: STA 13
; 2: LDI 2
; 3: STA 14
; 4: LDI 0
; 5: STA 15
; 6: LDA 14 (LOOP)
; 7: JZ 14  (saltar a END si contador es 0)
; 8: SUB 12 (decrementa contador)
; 9: STA 14
; 10: LDA 15 (carga resultado)
; 11: ADD 13 (suma A)
; 2: STA 250
; 4: LDI 4
; 6: STA 251
; 8: LDI 0
; 10: STA 252
; 12: LDA 250 (LOOP)
; 14: JZ 34  (saltar a END si contador es 0)
; 16: SUB 253 (decrementa contador)
; 18: STA 250
; 20: LDA 252 (carga resultado)
; 22: ADD 251 (suma A)
; 24: STA 252
; 26: JMP 12  (vuelta al LOOP)
; 34: LDA 252 (END)
; 36: OUT
; 38: HLT

LDI 3    ; Multiplicador
STA 250   ; Mem[250] = 3 (contador)
LDI 2    ; Multiplicando
STA 251   ; Mem[251] = 4 (sumando)
LDI 0    ; Inicializa acumulador en 0
STA 252   ; Mem[252] = 0 (resultado)

; --- INICIO DEL BUCLE (Address 12) ---
; Cargamos el contador
LDA 250
JZ 28    ; Si contador es 0, salta al OUT en address 28 (14 instrucciones * 2)

; Restamos 1 al contador
SUB 253  ; (Mem[253] tendra que inicializarse en 1)
STA 250

; Sumamos al resultado
LDA 252
ADD 251
STA 252

JMP 12   ; Vuelve al inicio del bucle

; --- FIN DEL BUCLE (Address 34) ---
LDA 252
OUT
HLT

253: 1    ; Constante 1
