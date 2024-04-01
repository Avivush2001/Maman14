; file error7.as
.entry LIST1
.extern W 
.define sz = 2 
; should print error for this
; at address 0102
MAIN:	mov r7, LIST1[sz]
LOOP: 	jmp W 
	prn #-5
	mov STR[5], STR[2] 
	sub r1, r4
	cmp K, #sz
	bne W
L1: 	inc L3 
.entry LOOP
	bne LOOP 
END: hlt
.define len = 4
STR: .string "abcdef" 
LIST: .data 6, -9, len 
K: .data 22 
.extern L3
; error file, specifically
; tests undefined entry label