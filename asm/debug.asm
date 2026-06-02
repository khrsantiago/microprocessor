INIT:
    ; Base VRAM (256)
    LDI R0, 127
    LDI R1, 1
    ADD R0, R1
    NOP
    ADD R0, R0
    ST [50], R0
    
    ; Color Verde
    LDI R0, 63
    NOP
    ADD R0, R0
    NOP
    ADD R0, R0
    NOP
    ADD R0, R0
    NOP
    ADD R0, R0
    NOP
    ADD R0, R0
    ST [51], R0

    ; Draw at X=14, Y=1
    LDI R0, 14 ; X
    NOP
    NOP
    LDI R1, 1 ; Y
    NOP
    NOP

    LDI R2, 0
    NOP
    NOP
    ADD R2, R1
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    NOP
    ADD R2, R2
    
    LD R3, [50]
    NOP
    ADD R2, R3
    ADD R2, R0
    LD R3, [51]
    NOP
    ST [R2], R3

    OUT R0
    HLT
