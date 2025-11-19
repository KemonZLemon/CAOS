addi $t1,$zero,8
LOOP: sll $t3,$t1,2
add $t2,$a0,$t3
lw $t8,0($t2)
addi $t1,$t1,1
bne $t8,$a1,LOOP
