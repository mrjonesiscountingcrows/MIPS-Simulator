lui $t1, 0x1001
ori $t1, $t1, 0x0000
addi $t2, $0, 10
sw $t2, 0($t1)
lw $t3, 0($t1)
