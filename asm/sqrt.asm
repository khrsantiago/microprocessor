; sqrt.asm - Cálculo de Raíz Cuadrada Entera aproximada por defecto.
; Utiliza el algoritmo de "restas de números impares consecutivos".
; Principio: n^2 es la suma de los primeros n números impares (1+3+5+...+(2n-1)).

; --- Mapa de registros ---
; R0: N (El número al que le queremos calcular la raíz)
; R1: Odd (El número impar actual a restar: 1, 3, 5, 7, ...)
; R2: Root (El contador de cuántas restas hemos hecho, que será la raíz)
; R3: Temporal para constantes de incremento

; --- Configuración inicial para N = 225 ---
; Como LDI solo admite 8 bits inmediatos, sumamos dos valores para obtener 225.
LDI R0, 112   ; Cargamos 112 en R0
LDI R1, 113   ; Cargamos 113 en R1
ADD R0, R1    ; R0 = 112 + 113 = 225 (N = 225)

LDI R1, 1     ; Primer número impar a restar (Odd = 1)
LDI R2, 0     ; Inicializamos la raíz en 0 (Root = 0)
LDI R3, 2     ; Constante 2 para generar el siguiente impar (Odd += 2)

LOOP:
SUB R0, R1    ; Restamos el impar actual a N (R0 = N - Odd)
JZ  FOUND     ; Si el resultado es exactamente 0, la raíz es perfecta. Ir a FOUND.
JC  CONTINUE  ; Si hubo acarreo (resultado positivo), el impar cabe. Ir a CONTINUE.
JMP END       ; Si el resultado fue negativo (N < Odd), terminamos. Ir a END.

CONTINUE:
LDI R3, 1     ; Cargamos la constante 1 temporalmente
ADD R2, R3    ; Incrementamos la raíz (Root++)
OUT R2        ; Enviamos el progreso actual al registro de salida
LDI R3, 2     ; Volvemos a cargar la constante 2 para el impar
ADD R1, R3    ; Generamos el siguiente número impar (Odd = Odd + 2)
JMP LOOP      ; Volver al inicio del ciclo

FOUND:
LDI R3, 1     ; En caso de raíz perfecta, sumamos la última resta exitosa
ADD R2, R3    ; Root++

END:
OUT R2        ; Mostrar la raíz cuadrada final (para 225 debería ser 15)
HLT           ; Detener el procesador
