; blinker.asm - Hardcoded Conway's Blinker for KHR-16
; Screen: 16x16, VRAM start: 256
 
; Permanent Constants
LDI R3, 1      ; R3 = 1
LDI R1, 64
NOP
NOP
ADD R1, R1     ; R1 = 128
NOP
NOP
ADD R1, R1     ; R1 = 256 (Base VRAM)
NOP
NOP
 
; Draw Center (136 + 256 = 392)
LDI R2, 68
NOP
NOP
ADD R2, R2     ; R2 = 136
NOP
NOP
ADD R2, R1     ; R2 = 392
NOP
NOP
LDI R0, 1
ST [R2], R0    ; Center Pixel ON
 
MAIN:
    ; === STATE 1: HORIZONTAL (391, 393) ===
    LDI R0, 1      ; Color ON
    
    ; 391
    LDI R2, 68
    NOP
    NOP
    ADD R2, R2
    NOP
    NOP
    SUB R2, R3     ; 135
    NOP
    NOP
    ADD R2, R1     ; 391
    NOP
    NOP
    ST [R2], R0
    
    ; 393
    LDI R2, 68
    NOP
    NOP
    ADD R2, R2
    NOP
    NOP
    ADD R2, R3     ; 137
    NOP
    NOP
    ADD R2, R1     ; 393
    NOP
    NOP
    ST [R2], R0
    
    ; Clear State 2 (376, 408)
    LDI R0, 0      ; Color OFF
    
    ; 376 (120 + 256)
    LDI R2, 120
    NOP
    NOP
    ADD R2, R1
    NOP
    NOP
    ST [R2], R0
    
    ; 408 (152 + 256)
    LDI R2, 76
    NOP
    NOP
    ADD R2, R2
    NOP
    NOP
    ADD R2, R1
    NOP
    NOP
    ST [R2], R0
    
    LDI R0, 1      ; Restore ON for OUT
    OUT R0
    
    ; Delay
    LDI R2, 20
D1:
    SUB R2, R3
    NOP
    NOP
    JZ D1E
    JMP D1
D1E:
 
    ; === STATE 2: VERTICAL (376, 408) ===
    LDI R0, 1      ; Color ON
    
    ; 376
    LDI R2, 120
    NOP
    NOP
    ADD R2, R1
    NOP
    NOP
    ST [R2], R0
    
    ; 408
    LDI R2, 76
    NOP
    NOP
    ADD R2, R2
    NOP
    NOP
    ADD R2, R1
    NOP
    NOP
    ST [R2], R0
    
    ; Clear State 1 (391, 393)
    LDI R0, 0      ; Color OFF
    
    ; 391
    LDI R2, 68
    NOP
    NOP
    ADD R2, R2
    NOP
    NOP
    SUB R2, R3
    NOP
    NOP
    ADD R2, R1
    NOP
    NOP
    ST [R2], R0
    
    ; 393
    LDI R2, 68
    NOP
    NOP
    ADD R2, R2
    NOP
    NOP
    ADD R2, R3
    NOP
    NOP
    ADD R2, R1
    NOP
    NOP
    ST [R2], R0
    
    LDI R0, 1
    OUT R0
    
    ; Delay
    LDI R2, 20
D2:
    SUB R2, R3
    NOP
    NOP
    JZ D2E
    JMP D2
D2E:
    
    JMP MAIN
