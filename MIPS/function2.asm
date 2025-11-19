.data


.text
main:
addi $a1, $zero, 50	#first arguement/parameter
addi $a2, $zero, 100	#second

jal addNums

li $v0, 1
addi $a0, $v1, 0

#tells the system the program is done
li $v0, 10
syscall

addNums:
add $v1, $a1, $a2

jr $ra