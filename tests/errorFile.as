; file error1.as
.entry LIST 
.extern W 
.define sz = 2 
MAIN:	mov r3[2], IST[sz]
LOOP: 	jmp W 
	prn #-5
mov STR[9], SR[2] 
	sub r1, r4
	cmp K, #sz
	bne W
L1: 	inc L3 
.entry LOOP
	bne LOOP 
END: hlt
.define len = 
STR: .string "abcdef" 
ST: .string abcdef" 
LIST: .data 6, -9, len 
K: .data 22 
.extern L3

; error file a few notes:
; if an error is encountered
; in a line in the first
; stage it will not be coded
; in the memory and stage 2
; errors in that
; line will not show up