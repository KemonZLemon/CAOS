.data
number1: .word 5
number2: .word 10
number3: .word 20
number4: .word 8

.text

#add
lw $t0, number1($zero)
lw $t1, number2($zero)
add $t2, $t0, $t1		#t2 = t0 + t1
li $v0, 1
add $a0, $zero, $t2
syscall 

#subtract
lw $s0, number3	#s0 = 20
lw $s1, number4	#s1 = 8
sub $t0, $s0, $s1	
li $v0, 1
move $a0, $t0
syscall

#3 ways to multiply
addi $s0, $zero, 10
addi $s1, $zero, 4

mul $t0, $s0, $s1
li $v0, 1
add $a0, $zero, $t0
syscall

addi $t0, $zero, 2000
addi $t1, $zero, 10

mult $t0, $t1 #gets stored in lo-hi register
mflo $s0	#stores lo to s0
li $v0, 1
add $a0, $zero, $s0
syscall

addi $s0, $zero, 4

sll $t0, $s0, 2	#s0 * 2^2 = 4 * 4 = 16
li $v0, 1
add $a0, $zero, $t0
syscall

#divide
addi $t0, $zero, 30
addi $t1, $zero, 5

div $s0, $t0, $t1

li $v0, 1
add $a0, $zero, $s0
syscall

addi $t0, $zero, 30
addi $t1, $zero, 5

div $s0, $t0, 10
li $v0, 1

add $a0, $zero, $s0
syscall

addi $t0, $zero, 30
addi $t1, $zero, 7

div $t0, $t1	#quotient will be stored in lo, remainder will be stored in hi

mflo $s0	#quotient	
mfhi $s1	#remainder
li $v0, 1
add $a0, $zero, $s1
syscall