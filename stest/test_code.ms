; The reset vector tells the CPU where to find the system reset routine.
; ./tmpx -i test_code.ms -o test_code.prg
* = $1000

lda #00
sta $42

start
inc $42
ldx $42
inx 
jmp start