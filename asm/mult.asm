; mult.asm - Multiplicación de dos números mediante sumas repetidas (3 * 2)
; Este programa utiliza un bucle para realizar la multiplicación.

; --- Inicialización de registros ---
; R0: Contador de iteraciones (Multiplicador)
; R1: Valor a sumar (Multiplicando)
; R2: Acumulador del resultado
; R3: Constante 1 para decrementar el contador

LDI R0, 3     ; Queremos multiplicar por 3
LDI R1, 2     ; Queremos sumar el valor 2
LDI R2, 0     ; Inicializamos el resultado en 0
LDI R3, 1     ; Cargamos la constante 1 para el paso de decremento

LOOP:
ADD R2, R1     ; Acumulador = Acumulador + Multiplicando (R2 += R1)
SUB R0, R3     ; Decrementar el contador: R0 = R0 - 1
JZ  END         ; Si el resultado de la resta fue 0 (Contador llegó a 0), saltar a END
JMP LOOP        ; Si no es 0, repetir el ciclo de suma

END:
OUT R2         ; Muestra el resultado final en el display (debería ser 6)
HLT            ; Detener el procesador
