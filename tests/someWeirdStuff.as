; file someWeirdStuff.as
; double entry or extern should print errors
.entry LIST 
.entry LIST 
.extern W
.extern W 
.define sz = +2047 
MAIN:	mov r3, LIST[0]
LOOP: 	jmp W
	prn #-5
	mov STR[0], STR[0] 
	sub r1, r4
	cmp K, #sz
	bne W
L1: 	inc L3 
; label in wrong place should print a warning
DUMB1LABEL: .entry LOOP
	bne LOOP 
END: hlt
DUMB2LABEL: .define len = -2048

; testing weird strings and labels
STR: .string ""
Str: .string """
sTR: .string "\0"
StR: .string "\n"
LIST: .data +8191, -8192, len 
K: .data 22 
.extern L3
; file for testing weird stuff
; that shouldn't cause
; fatal errors