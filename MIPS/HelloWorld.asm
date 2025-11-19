# Every MIPS program has 2 sections: data and text

.data	#variables
myMessage: .asciiz "Hello World\n"
myChar: .byte 'm'
age: .word 19
PI: .float 3.14
myDouble: .double 6.676767
zeroDouble: .double 0.0

.text	#instructions

li $v0, 4
la $a0, myMessage
syscall

li $v0, 4
la $a0, myChar
syscall

li $v0, 1
lw $a0, age
syscall

li $v0, 2
lwc1 $f12, PI 
syscall

ldc1 $f2, myDouble
ldc1 $f0, zeroDouble
li $v0, 3
add.d $f12, $f2, $f0
syscall