.data
ARRAY:  .word 1, 7, 23, 23, 41, 80, 90, 2, 67, 69, 33, 27, 60, 11, 5, 999, 23, 77, 88, 19
N:      .word 15                   
msg:    .asciiz "Sum of odd numbers = "
newline:.asciiz "\n"

.text
.globl main

main:
    # Load addresses and initial values
    la  $t0, ARRAY        # $t0 = base address of ARRAY
    lw  $t1, N            # $t1 = N (number of elements to check)
    li  $t2, 0            # $t2 = counter (starts at 0)
    li  $t3, 0            # $t3 = accumulator for sum

# Loop through first N elements
loop:
    beq $t2, $t1, done    # if counter == N, exit loop

    lw  $t4, 0($t0)       # load current ARRAY element
    andi $t5, $t4, 1      # check if odd (bitwise AND with 1)
    beqz $t5, skip_add    # if even, skip addition

    add  $t3, $t3, $t4    # if odd, add to sum

skip_add:
    addi $t0, $t0, 4      # move to next array element
    addi $t2, $t2, 1      # increment counter
    j loop                # repeat

# Print result
done:
    li  $v0, 4
    la  $a0, msg
    syscall

    li  $v0, 1
    move $a0, $t3
    syscall

    li  $v0, 4
    la  $a0, newline
    syscall

    # exit
    li  $v0, 10
    syscall
