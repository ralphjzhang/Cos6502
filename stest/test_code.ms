; The reset vector tells the CPU where to find the system reset routine.
; ./tmpx -i test_code.ms -o test_code.prg
* = $1000

lda #$FF

start
sta $90
sta $8000
eor #$CC
jmp start