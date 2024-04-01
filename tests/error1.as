; file error1.as
.entry LIST 
.extern W 
.define sz = 2 
; should print error
; at address 102
MAIN:	mov r3, IST[sz]
LOOP: 	jmp W 
	prn #-5
; should print error
; at addresses 109 111
	mov ST[5], SR[2] 
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
; error file, specifically tests
; undefined labels in operations
; should print multiple errors