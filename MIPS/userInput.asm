.data
prompt: .asciiz "Enter your age: "
message: .asciiz "\nYour age is "

.text

#prompt
li $v0, 4
la $a0, prompt
syscall

#get user input
li $v0, 5
syscall

#store input
move $t0, $v0

#display
li $v0, 4
la $a0, message
syscall

#print or show the age
li $v0, 1
move $a0, $t0
syscall