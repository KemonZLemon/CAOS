.data
message: .asciiz "The numbers are equal.\n"
message2: .asciiz "Nothing happened.\n"
message3: .asciiz "The numbers are different\n"
.text
main:
addi $t0, $zero, 20
addi $t1, $zero, 20

beq $t0, $t1, numEqual

addi $t0, $zero, 67
bne $t0, $t1, numDiff

li $v0, 10
syscall

numEqual:
li $v0, 4
la $a0, message
syscall

numDiff:
li $v0, 4
la $a0, message3
syscall

