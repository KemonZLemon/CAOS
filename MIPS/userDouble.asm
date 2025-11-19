.data
prompt: .asciiz "Enter the value of e: "

.text

#display message
li $v0, 4
la $a0, prompt
syscall

#read input
li $v0, 7
syscall

#display input
li $v0, 3
add.d $f12, $f0, $f2
syscall	
