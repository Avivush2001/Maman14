; file error2.as
; should throw an error here
.entry 2LIST 
.extern W 
.define sz = 2 
; shouldn't throw an error here
1MAIN:	mov r3, LIST[sz]
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
; tests illegal label name