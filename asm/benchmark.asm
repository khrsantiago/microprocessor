; benchmark.asm - Test de Rendimiento de Pipeline (Modernizado para GPR)
; Mide la velocidad de ejecución de un bucle de cuenta regresiva de 100 a 0.

; --- Inicialización ---
; R0: Contador regresivo
; R3: Constante 1 para el decremento
LDI R0, 100      ; Iniciamos el contador en 100
ST [10], R0      ; (Opcional) Guardamos estado en memoria
LDI R3, 1        ; Constante 1 para restar

; --- INICIO DEL BUCLE ---
LOOP:
LD R0, [10]      ; Recuperar el contador de la RAM (Añade una dependencia de memoria)
SUB R0, R3       ; Decrementar el contador: R0 = R0 - 1
ST [10], R0      ; Guardar el nuevo valor en RAM

; --- Comprobación de Condición ---
JZ END           ; Si R0 llegó a 0, salir del bucle
OUT R0           ; Mostrar el progreso actual
JMP LOOP         ; Repetir el ciclo (Salto incondicional)

; --- FIN DEL PROGRAMA ---
END:
HLT              ; Detener ejecución
