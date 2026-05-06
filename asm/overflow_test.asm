; Test de Overflow (V)
LDI R0, 127
LDI R1, 1
ADD R0, R1    ; 127 + 1 = 128 (Signed: 127 + 1 = -128 -> V=1)
OUT R0

LDI R0, -128
LDI R1, -1
ADD R0, R1    ; -128 + -1 = -129 (Signed: -128 + -1 = 127 -> V=1)
OUT R0

LDI R0, 254
LDI R1, 2
ADD R0, R1    ; 254 + 2 = 256 (Signed: -2 + 2 = 0 -> V=0, C=1)
OUT R0

HLT
