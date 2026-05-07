; test2.asm - Test de regresión para validación de arquitectura
; Un programa corto para verificar el flujo básico de instrucciones GPR.

LDI R0, 7        ; Paso 1: Cargar constante 7 en R0
ST [15], R0      ; Paso 2: Guardar el valor 7 en la posición 15 de la RAM
LDI R1, 3        ; Paso 3: Cargar constante 3 en R1
ADD R0, R1       ; Paso 4: Sumar R0 = R0 + R1 (7 + 3 = 10)
OUT R0           ; Paso 5: Mostrar el resultado final 10
HLT              ; Paso 6: Detener el procesador
