;This program counts from 10 to 0
	.ORIG x3000   	
START	ADD R1, R1, #-1
	NOT R1, R1
	STB R2, R3, #-5
	RSHFA R4, R5, #9
	TRAP x25
	LEA R1, TEST
	BRn START
	JSR TEST
TEST .FILL x3
	.END			;The pseudo-op, delimiting the source program