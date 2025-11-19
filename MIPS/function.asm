.data
message: .ascii "Hi, everybody.\nMy name is Rob.\n"

.text
main:
jal displayMessage

#print 5
addi $s0, $zero, 5
li $v0, 1
add $a0, $zero, $s0
syscall

#tells the system the program is done
li $v0, 10
syscall

displayMessage:
li $v0, 4
la $a0, message
syscall

jr $ra