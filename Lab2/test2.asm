;=====================================================================
; EE 460N Lab 2 -- HARD test case
;
; Every operate instruction in BOTH addressing modes (register and
; imm5), with positive and negative operands in each.  Plus:
;
;   .FILL #-1 loaded from memory
;   Low16bits truncation  (x8000 + x8000 must give x0000/Z, not x10000)
;   Low16bits truncation on LSHF and on -1 + -1
;   imm5 at both extremes (#15 and #-16)
;   PC+offset:  LEA forward, LEA backward, BR both ways, JSR backward
;   LEA proved not to touch the condition codes (branch depends on it)
;   every BR mask: n, z, p, np, zp, nz, nzp -- taken AND not taken
;   LDB/LDW/STB/STW at even and ODD byte addresses
;   negative offset6 on both LDB and LDW
;   LDB sign-extension on a byte with bit 7 set, and one without
;   JSRR and JMP to addresses LOADED FROM MEMORY (not LEA'd)
;   JSRR R7 -- base and link register are the same
;   shift by 0 (RSHFA #0)
;   memory written and read back for mdump verification
;
; Data lives at x5000 -- clear of the trap vector table (x0000-x01FF),
; the interrupt vector table (x0200-x03FF), and the program (x3000+).
;
; Run:  ./simulate test2.obj mem2.asm
;
; Correct run = exactly 98 instructions.  Any bad branch lands on
; BADBR, which halts early with R0 = xFFFF.
;=====================================================================

        .ORIG   x3000

        BRnzp   START

;--------------------------------------------------------------
; Subroutines and jump targets, placed FIRST so calls to them are
; BACKWARD -- a backward JSR is the only thing that catches a
; PCoffset11 truncated to 10 bits.
;--------------------------------------------------------------
SUB1    ADD     R3, R3, #1
        RET

SUB2    ADD     R3, R3, #2
        RET

SUB3    ADD     R3, R3, #4      ; reached by JSRR, address loaded from mem
        RET

JTGT    ADD     R4, R4, #8      ; reached by JMP, address loaded from mem
        BRnzp   JBACK           ; JMP has no linkage, so branch back

;--------------------------------------------------------------
; Setup
;--------------------------------------------------------------
START   LEA     R5, DPTR        ; forward LEA
        LDW     R5, R5, #0      ; R5 = x5000
        LEA     R7, NEG1
        LDW     R7, R7, #0      ; R7 = xFFFF  <- the .FILL #-1

;--------------------------------------------------------------
; ADD: imm5 at both extremes, register mode, negative + negative
;--------------------------------------------------------------
        AND     R0, R0, #0      ; R0 = x0000            Z
        ADD     R0, R0, #15     ; R0 = x000F            P   imm5 max
        ADD     R1, R0, #-16    ; R1 = xFFFF            N   imm5 min
        ADD     R2, R0, R1      ; R2 = x000E            P   reg, pos+neg
        AND     R3, R3, #0      ; R3 = x0000            Z
        ADD     R3, R3, #-1     ; R3 = xFFFF            N
        ADD     R4, R3, R1      ; R4 = xFFFE            N   neg+neg, truncated

;--------------------------------------------------------------
; Low16bits: build x8000, then overflow 16 bits on purpose
;--------------------------------------------------------------
        AND     R0, R0, #0      ; R0 = x0000            Z
        ADD     R0, R0, #1      ; R0 = x0001            P
        LSHF    R0, R0, #15     ; R0 = x8000            N
        ADD     R1, R0, R0      ; x10000 -> x0000       Z   <-- TRUNCATION

;--------------------------------------------------------------
; AND: imm5 negative (all ones), imm5 positive, register
;--------------------------------------------------------------
        AND     R2, R0, #-1     ; R2 = x8000            N   identity
        AND     R3, R0, #15     ; R3 = x0000            Z
        AND     R4, R0, R0      ; R4 = x8000            N

;--------------------------------------------------------------
; XOR / NOT: imm5 negative (= NOT), imm5 positive, register
;--------------------------------------------------------------
        XOR     R1, R0, #-1     ; R1 = x7FFF            P
        NOT     R2, R0          ; R2 = x7FFF            P   same encoding
        XOR     R3, R0, #7      ; R3 = x8007            N
        XOR     R4, R0, R1      ; R4 = xFFFF            N

;--------------------------------------------------------------
; Shifts on a NEGATIVE value, so RSHFL and RSHFA must differ
;--------------------------------------------------------------
        AND     R0, R0, #0      ; R0 = x0000            Z
        ADD     R0, R0, #-8     ; R0 = xFFF8            N
        LSHF    R1, R0, #4      ; R1 = xFF80            N   bits shifted out
        RSHFL   R2, R0, #4      ; R2 = x0FFF            P   zero fill
        RSHFA   R3, R0, #4      ; R3 = xFFFF            N   sign fill
        RSHFA   R4, R0, #0      ; R4 = xFFF8            N   shift by zero

;--------------------------------------------------------------
; Loads.  R5 = x5000.
;   x5000 = x1234   bytes x34 x12
;   x5002 = xFF80   bytes x80 xFF
;   x5004 = xFFFF   bytes xFF xFF
;   x5006 = x7FFF   bytes xFF x7F
;--------------------------------------------------------------
        LDW     R0, R5, #0      ; R0 = x1234            P
        LDW     R1, R5, #2      ; MAR x5004, R1 = xFFFF N
        LDB     R2, R5, #0      ; byte x5000, R2 = x0034    P   even
        LDB     R3, R5, #1      ; byte x5001, R3 = x0012    P   ODD
        LDB     R4, R5, #5      ; byte x5005, R4 = xFFFF    N   ODD + SEXT
        LDB     R6, R5, #7      ; byte x5007, R6 = x007F    P   ODD, no SEXT

;--------------------------------------------------------------
; Negative offset6 on both LDW and LDB
;--------------------------------------------------------------
        ADD     R6, R5, #15     ; R6 = x500F            P
        ADD     R6, R6, #1      ; R6 = x5010            P   (imm5 max is 15)
        LDW     R0, R6, #-8     ; MAR x5000, R0 = x1234     P
        LDB     R1, R6, #-11    ; MAR x5005, R1 = xFFFF     N
        LDB     R2, R6, #0      ; byte x5010, R2 = xFFFF    N
        LDB     R3, R6, #1      ; byte x5011, R3 = xFF80    N   ODD + SEXT

;--------------------------------------------------------------
; Stores, each read back
;--------------------------------------------------------------
        LDW     R0, R5, #3      ; R0 = x7FFF            P
        STW     R0, R5, #6      ; MEM[x500C] = x7FFF
        LDW     R1, R5, #6      ; R1 = x7FFF            P   verify STW
        LDB     R2, R5, #0      ; R2 = x0034            P
        STB     R2, R5, #14     ; mem[x500E] = x34          even byte
        AND     R3, R3, #0
        ADD     R3, R3, #-1     ; R3 = xFFFF            N
        STB     R3, R5, #15     ; mem[x500F] = xFF          ODD byte
        LDW     R4, R5, #7      ; R4 = xFF34            N   verifies BOTH STBs

;--------------------------------------------------------------
; BR: every mask, taken and not taken.  CC is N here.
;--------------------------------------------------------------
        BRz     BADBR           ; NOT taken
        BRp     BADBR           ; NOT taken
        BRn     B1              ; TAKEN
        BRnzp   BADBR

B1      AND     R0, R0, #0      ; R0 = x0000            Z
        BRn     BADBR           ; NOT taken
        BRp     BADBR           ; NOT taken
        BRz     B2              ; TAKEN
        BRnzp   BADBR

B2      ADD     R0, R0, #5      ; R0 = x0005            P
        BRn     BADBR           ; NOT taken
        BRz     BADBR           ; NOT taken
        BRp     B3              ; TAKEN
        BRnzp   BADBR

B3      ADD     R0, R0, #-5     ; R0 = x0000            Z
        BRnp    BADBR           ; NOT taken   mask 101
        BRzp    B4              ; TAKEN       mask 011
        BRnzp   BADBR

B4      ADD     R0, R0, #-1     ; R0 = xFFFF            N
        BRzp    BADBR           ; NOT taken   mask 011
        BRnz    B5              ; TAKEN       mask 110
        BRnzp   BADBR

;--------------------------------------------------------------
; Backward branch loop
;--------------------------------------------------------------
B5      AND     R1, R1, #0      ; R1 = 0                Z
        ADD     R2, R1, #3      ; R2 = 3                P
LOOP    ADD     R1, R1, #1
        ADD     R2, R2, #-1
        BRp     LOOP            ; backward; taken twice, then falls through
                                ; leaves R1 = 3, R2 = 0, Z

;--------------------------------------------------------------
; LEA must NOT set condition codes.  CC is Z here; if LEA wrongly
; sets P from x30CC, the BRz below fails and we land on BADBR.
;--------------------------------------------------------------
        LEA     R6, DPTR        ; R6 = x30CC, CC must stay Z
        BRz     LEAOK           ; TAKEN only if LEA left the CC alone
        BRnzp   BADBR

;--------------------------------------------------------------
; JSR / JSRR / JMP / RET
;--------------------------------------------------------------
LEAOK   AND     R3, R3, #0      ; R3 = 0
        JSR     SUB1            ; BACKWARD JSR.  R7 = x30B4, PC = x3002
                                ; R3 -> 1

        LDW     R6, R5, #4      ; R6 = MEM[x5008] = x300A   address from memory
        JSRR    R6              ; R7 = x30B8, PC = x300A
                                ; R3 -> 5

        LEA     R7, SUB2        ; backward LEA; R7 = x3006
        JSRR    R7              ; ALIASING: base and link are both R7.
                                ; PC = old R7 = x3006, new R7 = x30BC
                                ; R3 -> 7

        LDW     R6, R5, #5      ; R6 = MEM[x500A] = x300E   address from memory
        JMP     R6              ; PC = x300E
        ADD     R0, R0, #1      ; must NOT execute

JBACK   STW     R3, R5, #9      ; MEM[x5012] = x0007  (checked with mdump)
        TRAP    x25             ; R7 = x30C6, PC = x0000, halt

;--------------------------------------------------------------
BADBR   AND     R0, R0, #0
        ADD     R0, R0, #-1     ; R0 = xFFFF = "a branch went wrong"
        TRAP    x25

DPTR    .FILL   x5000
NEG1    .FILL   #-1

        .END
