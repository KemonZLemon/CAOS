addi $t7,$zero, 40 
addi $t0, $zero, 0    
addi $t1, $zero, 0
addi $a0, $zero, 1
addi $t4, $a0, 0


loop:
addi $t7,$t7, -1


add $a0, $t4, $zero
li $v0, 1
syscall

add $t0, $zero, $t1
add $t1, $zero, $a0

add $a0, $t0, $t1
add $t4, $a0, $zero

bne $t7, $zero, loop