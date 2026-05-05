; sqrt.asm - The Master Demonstration Program
; Algoritmo: Raíz Cuadrada Entera por método de restas sucesivas de números impares.
; Encuentra qué numero elevado al cuadrado construyó nuestra cifra N.
; ¡Testea TODAS las capacidades del procesador y las 11 instrucciones del ISA!

; --- MEMORIA DE CONSTANTES ---
LDI 1 
STA 201      ; Mem[201] = 1 (Constante)
LDI 2 
STA 202      ; Mem[202] = 2 (Constante)

; --- INICIALIZACION DE VARIABLES ---
LDI 225      ; N = 225 (Queremos encontrar la raiz cuadrada de 225. Debe ser 15)
STA 100      ; Mem[100] = N
LDI 1       
STA 101      ; Mem[101] = Odd (Siguiente impar a restar: 1, 3, 5, 7...)
LDI 0       
STA 102      ; Mem[102] = Root (Contador de raiz, empieza en 0)

; --- MAIN LOOP ---
LDA 100      ; 20: Carga N
SUB 101      ; 22: ALU ejecuta N - Odd
JZ 50        ; 24: [Z Flag]: Si N - Odd = 0 -> Saltar a PERFECT_ROOT a la dir 50
JC 30        ; 26: [C Flag]: Si Carry=1 (N >= Odd) -> Saltar a CONTINUE a la dir 30
JMP 62       ; 28: [Default]: Si Carry=0 (N < Odd) -> Saltar a END a la dir 62

; --- CONTINUE BLOCK (Dir 30) ---
STA 100      ; Guardar N de vuelta (descontado)

LDA 102      ; Carga Root actual
ADD 201      ; Root = Root + 1 (Aumentar el contador de raíz)
STA 102      

LDA 101      ; Carga Odd actual
ADD 202      ; Odd = Odd + 2 (Generar el siguiente número impar en la escalera)
STA 101      

LDA 102      
OUT          ; Muestra el progreso
JMP 20       ; Volver al loop (no cabe un NOP acá, ocupa 2 bytes)

; --- PERFECT ROOT BLOCK (Dir 50) ---
STA 100      ; Actualiza N a 0
LDA 102      
ADD 201      
STA 102      ; Suma el iterador 1 última vez marcando la raiz

NOP          ; Ciclo para reajustar los latches térmicamente
JMP 62       ; Enviar la raiz encontrada al End (Dir 62)

; --- END BLOCK (Dir 62) ---
LDA 102      ; Entra la raiz descubierta final
OUT          ; Muestra la Raiz
HLT          ; Detener la ejecución
