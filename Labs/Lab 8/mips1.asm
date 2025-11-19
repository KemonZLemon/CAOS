.data
# (a) Allocate memory space for 100 integers and initialize SIZE
ARRAY: .space 400     # 100 integers * 4 bytes/integer = 400 bytes
SIZE:  .word 0        # Single integer, initialized to zero


prompt_msg: .asciiz "Enter an integer between 1 and 100 (inclusive): "
error_msg:  .asciiz "Invalid input. Please try again.\n"

.text
.globl main

main:
    # --- (b) and (c) Prompt user and validate input loop ---
input_loop:
    # Print the prompt message
    li $v0, 4              # Syscall code for print_string
    la $a0, prompt_msg     # Load address of prompt_msg
    syscall

    # Read the user input
    li $v0, 5              # Syscall code for read_int
    syscall                # The entered value is in $v0

    # Store the input in a temporary register
    move $t0, $v0          # $t0 = user_input

    # Check for lower bound (input < 1)
    slti $t1, $t0, 1       # $t1 = 1 if $t0 < 1, else 0
    bne $t1, $zero, input_error # Branch to error if $t0 < 1

    # Check for upper bound (input > 100)
    li $t2, 100            # Load 100 into $t2
    sgt $t1, $t0, $t2      # $t1 = 1 if $t0 > 100, else 0 (sgt = set greater than)
    bne $t1, $zero, input_error # Branch to error if $t0 > 100

    # If validation passes, move to the next step
    j input_validated

input_error:
    # Print the error message
    li $v0, 4              # Syscall code for print_string
    la $a0, error_msg      # Load address of error_msg
    syscall
    j input_loop           # Loop back to re-prompt the user

input_validated:
    # --- (d) Store the validated value in SIZE ---
    la $t3, SIZE           # Load address of SIZE
    sw $t0, 0($t3)         # Store the user's input ($t0) into SIZE
    # $t0 now holds the SIZE value

    # --- (e) Calculate and store perfect squares in ARRAY ---

    # Initialize loop variables:
    li $t4, 1              # i = 1 (first number to square)
    li $t5, 0              # array_index_offset = 0
    la $t7, ARRAY          # $t7 = base address of ARRAY

calculate_loop:
    # Check loop condition: i > SIZE?
    # Loop continues as long as i <= SIZE ($t4 <= $t0)
    sgt $t8, $t4, $t0      # $t8 = 1 if $t4 > $t0 (i > SIZE), else 0
    bne $t8, $zero, end_program # Exit loop if $t4 > $t0

    # Calculate the perfect square: square = i * i
    mul $t6, $t4, $t4      # $t6 = $t4 * $t4 (i * i)

    # Store the square into ARRAY[array_index_offset]
    add $t9, $t7, $t5      # $t9 = address of ARRAY + array_index_offset
    sw $t6, 0($t9)         # Store the square ($t6) at that address

    # Increment i (i++)
    addi $t4, $t4, 1

    # Increment array_index_offset by 4 (next word/integer)
    addi $t5, $t5, 4

    j calculate_loop       # Continue the loop

end_program:
    # Exit the program gracefully
    li $v0, 10             # Syscall code for exit
    syscall