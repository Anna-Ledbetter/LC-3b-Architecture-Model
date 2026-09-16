; ============================================================
; offset torture test - LC-3b
; exercises: LEA/BR/JSR PC-relative offsets (fwd, back, zero,
; self), all 8 BR variants, labels beginning with 'r',
; forward refs to .FILL labels, and imm5/offset6/amount4 edges
; ============================================================
        .ORIG x3000

; ---- LEA: offset 0, +1, self(-1), backward ----
A0      LEA R0, A1          ; offset  0
A1      LEA R1, A3          ; offset +1
A2      LEA R2, A2          ; offset -1 (self)
A3      LEA R3, A0          ; offset -4

; ---- all 8 BR variants, forward ----
        BRn   FWD
        BRz   FWD
        BRp   FWD
        BRnz  FWD
        BRnp  FWD
        BRzp  FWD
        BR    FWD
        BRnzp FWD
FWD     ADD R0, R0, #0

; ---- BR backward ----
BACK    ADD R1, R1, #1
        BRnzp BACK          ; offset -2
        BRz   BACK          ; offset -3

; ---- JSR forward, zero, backward ----
        JSR SUB1            ; forward +1
        JSR SUB1            ; forward  0
SUB1    RET
        JSR SUB1            ; backward -2

; ---- labels that start with 'r' (must NOT parse as registers) ----
R9      ADD R2, R2, #0
RESET   BRnzp R9            ; backward to r-label
        LEA R4, RESET       ; backward to r-label

; ---- forward ref to a .FILL label ----
        LEA R5, DAT1
        LDW R5, R5, #0

; ---- imm5 / offset6 / amount4 edges ----
        ADD R6, R6, #-16    ; imm5 min
        ADD R6, R6, #15     ; imm5 max
        AND R6, R6, #-1
        LDB R7, R6, #-32    ; offset6 min
        LDB R7, R6, #31     ; offset6 max
        LDW R7, R6, #-32
        STB R7, R6, #31
        STW R7, R6, #-1
        LSHF  R1, R2, #0    ; amount4 min
        LSHF  R1, R2, #15   ; amount4 max
        RSHFL R1, R2, #15
        RSHFA R1, R2, #15

; ---- trap ----
        TRAP x25
        HALT

; ---- data, incl. negative .FILL ----
DAT1    .FILL x4000
DAT2    .FILL #-1
DAT3    .FILL #0
DAT4    .FILL xFFFF
DAT5    .FILL #-32768
        .END