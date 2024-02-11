.define sz = 2
MAIN:   mov r3, LIST[sz] 
LOOP:   jmp  L1 
mcr  mmcr  
    cmp r3, #sz 
    bne END 
endmcr  
prn #-5 
mov STR[5], STR[2] 
sub r1, r4 
mmcr
L1: inc K 
    bne LOOP        
END:    hlt 
mcr  mmcr 
.define len = 4 
STR:    .string  “abcdef” 
endmcr
LIST:   .data 6, -9, len 
K:  .data 22
;Tried to define a macro that is already defined.
;Error will be printed.