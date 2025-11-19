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
# based on input parameter $a1. Uses exactly one branch (beq).
#
printb:
        addi $sp,$sp,-4      # Save $a0 because it's modified by la/syscall
        sw   $a0,0($sp)
        
        li   $v0,4           # Prepare syscall for print_str
        beq  $a1,$zero,is_zero # If $a1 is 0, branch to print zerotxt (one branch)

        # 'else' (is_one) block: $a1 is non-zero (bit is 1)
        la   $a0,onetxt      # Load address of onetxt ($) - modifies $a0
        j    print           # Jump to print and return
is_zero:
        # 'if' (is_zero) block: $a1 is zero (bit is 0)
        la   $a0,zerotxt     # Load address of zerotxt (-) - modifies $a0
print:
        syscall              # Execute print_str
        
        lw   $a0,0($sp)      # Restore $a0
        addi $sp,$sp,4       # Restore $sp
        jr   $ra             # Return to caller

###################################################################
# The bcount function counts the number of "one" bits in the given
# input parameter $a0 (32-bit word), using width $a2.
#
bcount: addi $sp,$sp,-28     # Save $a0, $ra, $s0, $s1, $s2, $s3, $s4 (7 * 4 = 28 bytes)
        sw   $a0,0($sp)      # 0: original word $a0 (for return)
        sw   $ra,4($sp)      # 4: return address $ra 
        sw   $s0,8($sp)      # 8: block count $s0 
        sw   $s1,12($sp)     # 12: main counter $s1 
        sw   $s2,16($sp)     # 16: block counter $s2
        sw   $s3,20($sp)     # 20: original width $s3 
        sw   $s4,24($sp)     # 24: shifted word $s4 (NEW)
        
        move $s4,$a0         # $s4 = word to be shifted (original $a0)
        move $s3,$a2         # $s3 = width $a2 (persistent storage for reset)
        li   $s0,0           # $s0: block '1' bit count
        li   $s1,32          # $s1: main 32-bit loop counter
        move $s2,$s3         # $s2: block width counter

loop:
        # Check for end of 32 bits
        beq  $s1,$zero,done_bcount 

        # 1. Process bit and print (Always runs)
        li   $t2,0x80000000
        and  $a1,$s4,$t2     # $a1 is based on $s4$ (the safe register)
        
        # We must copy $s4$ to $a0$ before the jal, so printb can save/restore $a0$
        # and $a0$ comes back with the shifted word value.
        move $a0,$s4
        jal  printb          
        move $s4,$a0         # Copy the restored $a0$ back to $s4$
        
        # 2. Count '1' bit
        beq  $a1,$zero,skip_inc_b   # If $a1$ is 0, skip increment
        addi $s0,$s0,1              # Increment '1' bit count in $s0$
skip_inc_b:
        # 3. Decrement block width counter
        sub  $s2,$s2,1              # Decrement block width counter $s2$
        
        # 4. Check if block is complete ($s2 is now 0)
        bne  $s2,$zero,shift_and_update_main_counter # If not 0, continue loop
        
        # 5. Block is complete, print results and reset counters
        # $a0$ is clobbered here, but $s4$ holds the data word state, so it is safe.
        la   $a0,txt1        # Print " --> '1' bits: "
        li   $v0,4
        syscall
        
        move $a0,$s0         # Print the count in $s0$
        li   $v0,1
        syscall
        
        la   $a0,newline     # Print '\n'
        li   $v0,4
        syscall
        
        li   $s0,0           # Reset '1' bit count
        move $s2,$s3         # $s2 = original $a2 (width)

shift_and_update_main_counter:
        # 6. Shift word and update 32-bit counter
        sll  $s4,$s4,1           # Shift $s4$, not $a0$
        sub  $s1,$s1,1           # decrement main 32-bit loop variable
        j    loop

done_bcount: 
        # Restore saved registers
        lw   $s4,24($sp)
        lw   $s3,20($sp)
        lw   $s2,16($sp)
        lw   $s1,12($sp)
        lw   $s0,8($sp)
        lw   $ra,4($sp)
        lw   $a0,0($sp)
        addi $sp,$sp,28
        jr   $ra             # return to caller

###################################################################
#
main:   addi $sp,$sp,-12
        sw   $a0,0($sp)      # save register $a0 on the stack
        sw   $a1,4($sp)      # save register $a1 on the stack
        sw   $ra,8($sp)      # save return address $ra on the stack

        li   $a2,11          # Set the correct width parameter $a2$ to 16 for secret2
        la   $s1,secret2     # Load the address of secret2
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