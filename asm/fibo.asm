; fibo.asm - Secuencia de Fibonacci hasta el límite de 16 bits firmado.
; Calcula la secuencia F(n) = F(n-1) + F(n-2) hasta que el valor sea negativo (desborde).

; --- Mapa de registros ---
; R0: F(n) - Número actual de la secuencia
; R1: F(n+1) - Siguiente número de la secuencia
; R2: Temporal para calcular la suma
; R3: No utilizado

LDI R0, 0        ; Primer número: F(0) = 0
LDI R1, 1        ; Segundo número: F(1) = 1

LOOP:
OUT R0           ; Mostrar el número actual de la secuencia
LDI R2, 0        ; Limpiamos R2 para usarlo como acumulador
NOP              ; Burbuja de pipeline para estabilidad de datos
ADD R2, R0       ; R2 = R0 (Cargar valor actual)
NOP              ; Burbuja de pipeline
ADD R2, R1       ; R2 = R0 + R1 (Calcular el siguiente número de la serie)

; --- Comprobación de límite ---
; En aritmética firmada de 16 bits, el valor máximo es 32,767.
; Al superar este valor, el bit 15 se activa y el número se interpreta como negativo.
JN  FIN          ; Si el resultado es negativo (bit 15 activo), hemos terminado.

; --- Preparación para la siguiente iteración (Desplazamiento) ---
LDI R0, 0        ; Limpiamos R0
NOP              ; Estabilidad de pipeline
ADD R0, R1       ; R0 = R1 (F(n) ahora es el que era F(n+1))
LDI R1, 0        ; Limpiamos R1
NOP              ; Estabilidad de pipeline
ADD R1, R2       ; R1 = R2 (F(n+1) ahora es la suma recién calculada)

JMP LOOP         ; Repetir el proceso

FIN:
OUT R0           ; Mostrar el último valor válido calculado (28,657)
HLT              ; Detener el procesador
