; The reset vector tells the CPU where to find the system reset routine.
; ./tmpx -i test_code.ms -o test_code.prg
* = $1000

loop
lda #0
beq loop
