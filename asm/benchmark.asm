; ==========================================================
; Benchmark KHR-8 (Test de Rendimiento de Pipeline)
; ==========================================================
; Este programa mide que tan rapido el procesador ejecuta
; un bucle de cuenta regresiva (de 100 a 0).
;
; Utiliza variables en la memoria RAM para mantener el estado.
; ==========================================================

LDI 100      ; Cargar el numero 100 en el Acumulador
STA 10       ; Guardar el contador en Mem[10]

LDI 1        ; Cargar el valor 1 para restar
STA 11       ; Guardar el decrementador en Mem[11]

; --- INICIO DEL BUCLE (Direccion ~ 0x08) ---
LDA 10       ; (Loop) Cargar el contador actual
SUB 11       ; Restar 1 al contador
STA 10       ; Guardar el nuevo valor del contador

; Comprobar si llegamos a 0
JZ 20        ; Si el resultado de la resta es 0 (Zero Flag), saltar al FIN (Addr 20)

; Si no es cero, imprimir el progreso y seguir en el bucle
OUT          ; Muestra el contador actual en el display
JMP 8        ; Saltar de vuelta al inicio del bucle (Direccion 8)

; --- FIN DEL PROGRAMA (Direccion ~ 0x14 / 20) ---
HLT          ; Detener ejecucion
