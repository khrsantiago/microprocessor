; sum.asm - Demostración de sumas simples y con signo.
; Este programa muestra cómo el procesador realiza sumas de 16 bits y gestiona literales negativos.

; --- 1. Suma básica: 7 + 3 = 10 ---
LDI R0, 7     ; Carga el valor 7 en el registro R0
LDI R1, 3     ; Carga el valor 3 en el registro R1
ADD R0, R1    ; R0 = R0 + R1 -> 7 + 3 = 10
OUT R0        ; Muestra el resultado (10) en el display

; --- 2. Suma mediana: 15 + 10 = 25 ---
LDI R0, 15    ; Carga el valor 15 en R0
LDI R1, 10    ; Carga el valor 10 en R1
ADD R0, R1    ; R0 = R0 + R1 -> 15 + 10 = 25
OUT R0        ; Muestra el resultado (25)

; --- 3. Suma con signo: -8 + 25 = 17 ---
LDI R0, -8    ; Carga -8 en R0 (El procesador realiza extensión de signo interna)
LDI R1, 25    ; Carga 25 en R1
ADD R0, R1    ; R0 = -8 + 25 = 17
OUT R0        ; Muestra el resultado (17)

; --- 4. Suma de negativos: -8 + -8 = -16 ---
LDI R0, -8    ; Carga -8 en R0
LDI R1, -8    ; Carga -8 en R1
ADD R0, R1    ; R0 = -8 + (-8) = -16
OUT R0        ; Muestra el resultado (-16)

; --- 5. Demostración de límite de 8 bits en LDI: 254 + 2 ---
; Nota: LDI carga valores de 8 bits, lo que permite representar de -128 a 255 (unsigned) o -128 a 127 (signed).
LDI R0, 254   ; Carga 254 en R0
LDI R1, 2     ; Carga 2 en R1
ADD R0, R1    ; R0 = 254 + 2 = 256. 
              ; En una arquitectura de solo 8 bits esto daría 0, 
              ; pero el KHR-16 es de 16 bits, por lo que mostrará 256 correctamente.
OUT R0        ; Muestra el resultado (256)

HLT           ; Finaliza el programa
