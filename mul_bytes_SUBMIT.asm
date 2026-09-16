.ORIG x3000
AND R0, R0, #0  ; R0 = 0
ADD R1, R0, #1  ; R1 = 1
ADD R6, R0, #0  ; R6 = 0
LEA R5, DATA    ; R5 = DATA
LDW R4, R5, #0  ; R4 = x3100
LDB R5, R4, #0  ; R5 = mem[x3100] = num1
LDB R2, R4, #1  ; R2 = mem[x3101]
BRz SKIP        ; if R2 = 0

LOOP ADD R6, R6, R5 ; R6 += R5=num1
ADD R2, R2, #-1     ; num2 -= 1
BRp LOOP

SKIP STB R6, R4, #2 ; R6 --> mem[x3102]
STB R0, R4, #3      ; R0=0 --> mem[x3103]
LEA R3, MASK        ; R3 = MASK
LDW R3, R3, #0      ; R3 = xFF00
AND R6, R6, R3      ; R6 = R6 & xFF00
BRz DONE
STB R1, R4, #3      ; R1=1 --> mem[x3103]
DONE HALT

DATA .FILL x3100
MASK .FILL xFF00
.End