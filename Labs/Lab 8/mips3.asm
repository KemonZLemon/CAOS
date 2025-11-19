
.data
ARRAY:  .word 12, 7, 25, 30, 41, 8, 9, 2, 55, 14, 3, 27, 60, 11, 5, 100, 23, 77, 88, 19
N:      .word 20
msg:    .asciiz "Sorted array:\n"
newline:.asciiz "\n"

.text
.globl main


main:
    # Load array base and size
    la   $a0, ARRAY        # A (array base address)
    li   $a1, 0            # lo = 0
    lw   $t0, N
    addi $a2, $t0, -1      # hi = N - 1

    # Call qsort(A, 0, N-1)
    jal  qsort

    # Print result
    li   $v0, 4
    la   $a0, msg
    syscall

    la   $t1, ARRAY
    lw   $t2, N

print_loop:
    beqz $t2, end_program
    lw   $a0, 0($t1)
    li   $v0, 1
    syscall

    li   $v0, 4
    la   $a0, newline
    syscall

    addi $t1, $t1, 4
    addi $t2, $t2, -1
    j print_loop

end_program:
    li $v0, 10
    syscall


# Arguments:
#   $a0 = base address of array A
#   $a1 = lo
#   $a2 = hi
qsort:
    # Base case: if (lo >= hi) return;
    bge  $a1, $a2, qsort_return

    # Save registers to stack
    addi $sp, $sp, -16
    sw   $ra, 0($sp)
    sw   $a1, 4($sp)
    sw   $a2, 8($sp)

    # Call partition(A, lo, hi)
    jal  part

    # Partition result (p) is in $v0
    move $s0, $v0           # p = return value

    # Call qsort(A, lo, p)
    lw   $a1, 4($sp)        # reload lo
    move $a2, $s0           # hi = p
    jal  qsort

    # Call qsort(A, p+1, hi)
    addi $a1, $s0, 1        # lo = p + 1
    lw   $a2, 8($sp)        # reload hi
    jal  qsort

    # Restore stack and return
    lw   $ra, 0($sp)
    addi $sp, $sp, 16
qsort_return:
    jr $ra


# Arguments:
#   $a0 = base address of array A
#   $a1 = lo
#   $a2 = hi
# Return:
#   $v0 = partition index (hi)
part:
    # Save caller registers
    addi $sp, $sp, -24
    sw   $ra, 0($sp)
    sw   $a1, 4($sp)
    sw   $a2, 8($sp)
    sw   $s0, 12($sp)
    sw   $s1, 16($sp)
    sw   $s2, 20($sp)

    # pivot = A[lo]
    sll  $t0, $a1, 2
    add  $t0, $a0, $t0
    lw   $t1, 0($t0)        # pivot in $t1

partition_loop:
    # while (A[lo] < pivot) lo++
lo_loop:
    sll  $t2, $a1, 2
    add  $t2, $a0, $t2
    lw   $t3, 0($t2)
    bge  $t3, $t1, hi_loop
    addi $a1, $a1, 1
    j lo_loop

    # while (A[hi] > pivot) hi--
hi_loop:
    sll  $t4, $a2, 2
    add  $t4, $a0, $t4
    lw   $t5, 0($t4)
    ble  $t5, $t1, check_exit
    addi $a2, $a2, -1
    j hi_loop

check_exit:
    # if (lo >= hi) return hi
    bge  $a1, $a2, part_return

    # swap A[lo] and A[hi]
    lw   $t6, 0($t2)       # A[lo]
    lw   $t7, 0($t4)       # A[hi]
    sw   $t7, 0($t2)
    sw   $t6, 0($t4)

    addi $a1, $a1, 1
    addi $a2, $a2, -1
    j partition_loop

part_return:
    move $v0, $a2

    # Restore registers and return
    lw   $ra, 0($sp)
    lw   $s0, 12($sp)
    lw   $s1, 16($sp)
    lw   $s2, 20($sp)
    addi $sp, $sp, 24
    jr   $ra
