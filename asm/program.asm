; Programa de ejemplo para SAP-1
; Realiza la suma de dos numeros y muestra el resultado

LDI 7    ; Carga 7 directamente en RegA
STA 15   ; Guarda ese 7 en la dirección 15 de la RAM
LDI 3    ; Carga 3 directamente en RegA
ADD 15   ; Suma lo que hay en 15 (el 7) al RegA (3 + 7 = 10)
OUT      ; Muestra 10
HLT
