.data
prompt: .asciiz "Hello, "
input: .space 20	#basically array of 20 since we read name as chars
.text

main:
#get input
li $v0, 8
la $a0, input
li $a1, 20
syscall

#display message
li $v0, 4
la $a0, prompt
syscall

#display input
li $v0, 4
la $a0, input
syscall


#end of main
li $v0, 10
syscall