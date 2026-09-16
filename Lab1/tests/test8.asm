.Orig x3000
one .Fill x13
two .fill #-5

AND R7, R7, #4
AND R7, R7, #-3

AND R3, R7, #2
AND R6, R7, #-1

LSHF R1, R2, #2
RSHFL R1, R2, #1
RSHFA R1, R2, #3

JMP R7
JSRR R4

RTI


LDW R1, R2, #3
STW R3, R4, #-2

HALT
.END