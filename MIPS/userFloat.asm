.data
message: .asciiz "Enter the value of pi: "
zero: .float 0.0
.text
lwc1 $f2, zero

#display message
li $v0, 4
la $a0, message
syscall

#read input
li $v0, 6
syscall

#display value
li $v0, 2
add.s $f12, $f0, $f2
syscall

