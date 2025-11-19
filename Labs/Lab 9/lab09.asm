# lab09.asm

.data
txt1:   .asciiz " --> '1' bits: "
newline:.asciiz "\n"

zerotxt:.asciiz "-"     
onetxt: .asciiz "$"     

secret: .word 0x00020000, 0x40070000
        .word 0x60048000, 0x90044000
        .word 0x88044000, 0x84043f00
        .word 0x820400c0, 0x8189807e
        .word 0x8233c003, 0x4461803e
        .word 0x248823d0, 0x150003d0
        .word 0xe600f020, 0x89006120
        .word 0x45000040, 0x250000c0
        .word 0x16000030, 0x0c000188
        .word 0x08000244, 0x1000043c
        .word 0x20000200, 0x20000100
        .word 0x20ff0100, 0x1300c200
        .word 0x12002180, 0x3c001fe0
        .word 0x00000000     # indicates end of secret data

secret2:.word 0x7808301c, 0x30363063
        .word 0x337f7f63, 0x8000737f
        .word 0x36303c3f, 0x36303333
        .word 0x737f8000, 0x7e3e3763
        .word 0x3c60363e, 0x3303737e
        .word 0x00000000     # indicates end of secret2 data

###################################################################
.text
.globl printb
.globl bcount
.globl main

###################################################################
# The printb procedure prints either the zerotxt or onetxt string
# based on input parameter $a1 -- change zerotxt and onetext above
#
printb:
        addi $sp,$sp,-4      # Save $a0 because it's modified by la/syscall
        sw   $a0,0($sp)
        
        li   $v0,4           # Prepare syscall for print_str
        beq  $a1,$zero,is_zero # If $a1 is 0, branch to print zerotxt (one branch)

        # 'else' (is_one) block: $a1 is non-zero (bit is 1)
        la   $a0,onetxt      # Load address of onetxt ($) - **modifies $a0**
        j    print           # Jump to print and return
is_zero:
        # 'if' (is_zero) block: $a1 is zero (bit is 0)
        la   $a0,zerotxt     # Load address of zerotxt (-) - **modifies $a0**
print:
        syscall              # Execute print_str
        
        lw   $a0,0($sp)      # Restore $a0
        addi $sp,$sp,4       # Restore $sp
        jr   $ra             # Return to caller

###################################################################
# The bcount function counts the number of "one" bits in the given
# input parameter $a0 (32-bit word), returning this count in $v0
#
# The function also prints each bit via the printb procedure
#
bcount: addi $sp,$sp,-8      # save registers $a0 (word) and $ra on the stack
        sw   $a0,0($sp)
        sw   $ra,4($sp)

        li   $t0,0           # use $t0 to keep track of the number of 1 bits
        li   $t1,32          # loop variable to iterate 32 times (word length)

loop:   li   $t2,0x80000000  # mask (clear) all but the leftmost bit
        and  $a1,$a0,$t2     # prepare argument $a1 for printb procedure
        jal  printb          # call the printb procedure (now $a0 is preserved)
        beq  $a1,$zero,skip  # if $a1 is 0, skip this bit
        addi $t0,$t0,1       # count this 1 bit
skip:   sll  $a0,$a0,1       # shift $a0 left one bit position
        sub  $t1,$t1,1       # decrement loop variable
        bne  $t1,$zero,loop

        # Print " --> '1' bits: "
        la   $a0,txt1
        li   $v0,4           # print_str syscall
        syscall

        # Print the count in $t0
        move $a0,$t0         # Move the final count ($t0) to $a0 for printing
        li   $v0,1           # print_int syscall
        syscall

        # Print '\n'
        la   $a0,newline
        li   $v0,4           # print_str syscall to print '\n'
        syscall

        move $v0,$t0         # return the count in $v0
        lw   $ra,4($sp)      # restore original $ra value
        lw   $a0,0($sp)      # restore original $a0 value
        addi $sp,$sp,8
        jr   $ra             # return to caller

###################################################################
#
main:   addi $sp,$sp,-12
        sw   $a0,0($sp)      # save register $a0 on the stack
        sw   $a1,4($sp)      # save register $a1 on the stack
        sw   $ra,8($sp)      # save return address $ra on the stack

#        li   $a0,8192        # test bcount procedure
#        jal  bcount
#        j    done

        la   $s1,secret
repeat: lw   $a0,0($s1)
        beq  $a0,$zero,done
        jal  bcount
        addi $s1,$s1,4       # advance to the next word
        j    repeat

done:   lw   $ra,8($sp)
        lw   $a1,4($sp)
        lw   $a0,0($sp)
        addi $sp,$sp,12      # Correctly restore $sp by 12
        addi $v0, $zero, 10
        syscall