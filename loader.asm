lis $4
.word 4
lis $6
.word 12

; read MIPS start address
sw $31, -4($30)
sub $30, $30, $4

lis $25
.word readWord
jalr $25

add $30, $30, $4
lw $31, -4($30)

; create copy of address in $5 and $10
add $5, $3, $0
add $10, $3, $0


; first header line
sw $31, -4($30)
sub $30, $30, $4

lis $25
.word readWord
jalr $25

add $30, $30, $4
lw $31, -4($30)


; second header line
sw $31, -4($30)
sub $30, $30, $4

lis $25
.word readWord
jalr $25

add $30, $30, $4
lw $31, -4($30)

add $29, $3, $0


; third header line
sw $31, -4($30)
sub $30, $30, $4

lis $25
.word readWord
jalr $25

add $30, $30, $4
lw $31, -4($30)

sub $28, $3, $6 ; code size



    ; store instructions
    add $7, $0, $0
    loop1:
        slt $8, $7, $28
        beq $8, $0, endloop1

        ; store instruction
            sw $31, -4($30)
            sub $30, $30, $4

            lis $25
            .word readWord
            jalr $25

            add $30, $30, $4
            lw $31, -4($30)

        
        ; print instruction
            add $1, $3, $0

            sw $31, -4($30)
            sub $30, $30, $4

            lis $25
            .word printHex
            jalr $25

            add $30, $30, $4
            lw $31, -4($30)

        ; post processing
            sw $1, 0($5)
            add $5, $5, $4

            add $7, $7, $4
            beq $0, $0, loop1
            

    endloop1:


; __________________________________
; process relocation

add $7, $28, $6

loop2:

    slt $8, $7, $29
    beq $8, $0, endloop2

    ; read format

        sw $31, -4($30)
        sub $30, $30, $4

        lis $25
        .word readWord
        jalr $25

        add $30, $30, $4
        lw $31, -4($30)

    ; read rel

        sw $31, -4($30)
        sub $30, $30, $4

        lis $25
        .word readWord
        jalr $25

        add $30, $30, $4
        lw $31, -4($30)

    ; perform relocation

        add $8, $10, $3
        sub $8, $8, $6
        
        lw $9, 0($8)
        add $9, $9, $10
        sub $9, $9, $6

        sw $9, 0($8)
    
    ; post processing

        add $7, $7, $4
        add $7, $7, $4

        beq $0, $0, loop2
        
endloop2:



; ___________________________________________________
; execute


lis $27
.word 0x111000

sw $4, -4($27)
sw $6, -8($27)
sw $10, -12($27)
sw $28, -16($27)
sw $29, -20($27)
sw $30, -24($27)
sw $31, -28($27)

jalr $10

lis $27
.word 0x111000

lw $4, -4($27)
lw $6, -8($27)
lw $10, -12($27)
lw $28, -16($27)
lw $29, -20($27)
lw $30, -24($27)
lw $31, -28($27)


; __________________________________
; print second time

    add $7, $0, $0

    loop3:

        slt $8, $7, $28
        beq $8, $0, endloop3

        ; print instruction

            lw $1, 0($10)

            sw $31, -4($30)
            sub $30, $30, $4

            lis $25
            .word printHex
            jalr $25

            add $30, $30, $4
            lw $31, -4($30)
        
        ; post processing
        add $10, $10, $4
        add $7, $7, $4
        beq $0, $0, loop3

    endloop3:


; quit

jr $31



readWord:
sw $1,  -4($30)
sw $2,  -8($30)
sw $4, -12($30)
sw $5, -16($30)
sw $6, -20($30)
sw $7, -24($30)
sw $8, -28($30)
lis $8
.word 28
sub $30, $30, $8
lis $4
.word 0x01000000
lis $3
.word 0x00010000
lis $2
.word 0x00000100
lis $1
.word 0xffff0004
lw $8, 0($1)
lw $7, 0($1)
lw $6, 0($1)
lw $5, 0($1)
multu $8, $4
mflo $8
multu $7, $3
mflo $7
multu $6, $2
mflo $6
add $4, $8, $7
add $3, $6, $5
add $3, $4, $3
lis $8
.word 28
add $30, $30, $8
lw $1,  -4($30)
lw $2,  -8($30)
lw $4, -12($30)
lw $5, -16($30)
lw $6, -20($30)
lw $7, -24($30)
lw $8, -28($30)
jr $31

printHex:
sw $1,  -4($30)
sw $2,  -8($30)
sw $3, -12($30)
sw $4, -16($30)
sw $5, -20($30)
sw $6, -24($30)
sw $7, -28($30)
sw $8, -32($30)
lis $8
.word 32
sub $30, $30, $8
lis $2
.word 0x10000000
lis $3
.word 0x10
lis $4
.word 0x30
lis $6
.word 10
lis $7
.word 7  
lis $8
.word 0xffff000c
PHloop:
divu $1, $2
mflo $1
slt $5, $1, $6 
bne $5, $0, PHskip
        add $1, $1, $7
PHskip: add $1, $1, $4 
sw $1, 0($8)
mfhi $1
divu $2, $3
mflo $2
bne $2, $0, PHloop
sw $6, 0($8)
lis $8
.word 32
add $30, $30, $8
lw $1,  -4($30)
lw $2,  -8($30)
lw $3, -12($30)
lw $4, -16($30)
lw $5, -20($30)
lw $6, -24($30)
lw $7, -28($30)
lw $8, -32($30)
jr $31
