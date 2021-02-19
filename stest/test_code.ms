; The reset vector tells the CPU where to find the system reset routine.
; ./tmpx -i test_code.ms -o test_code.prg
* = $1000

lda #0
clc
loop
    adc #8
    cmp #24
    bne loop

ldx #20
