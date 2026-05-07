; div.asm - División entera (División por restas sucesivas): 10 / 2 = 5
; Este programa calcula cuántas veces cabe el divisor en el dividendo.

; --- Mapa de registros ---
; R0: Dividendo (valor a dividir, se reduce en cada iteración)
; R1: Divisor (valor constante que resta al dividendo)
; R2: Cociente (contador de cuántas restas exitosas se han hecho)
; R3: Constante 1 para incrementar el cociente

; --- Inicialización ---
LDI R0, 10    ; Cargamos el dividendo (10)
LDI R1, 2     ; Cargamos el divisor (2)
LDI R2, 0     ; Inicializamos el cociente en 0
LDI R3, 1     ; Cargamos la constante 1 para incrementos

LOOP:
SUB R0, R1    ; Restamos el divisor al dividendo (R0 = R0 - R1)
JN END         ; Si el resultado es negativo, el divisor ya no cabe. Ir a END.
ADD R2, R3    ; Si no es negativo, incrementamos el cociente (R2++)
JZ END         ; Si el dividendo llegó exactamente a 0, ya terminamos. Ir a END.
JMP LOOP       ; Si el dividendo sigue siendo positivo, repetir el ciclo.

END:
OUT R2        ; Enviar el cociente final al display (debería ser 5)
HLT           ; Detener la ejecución