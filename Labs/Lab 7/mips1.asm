add  $a0, $zero, $zero
addi $t0, $zero, -1
addi $t7, $zero, 100


loop:
  addi $t0, $t0, 2
  addi $t1, $t1, 1
  add  $a0, $a0, $t0
  bne  $t1, $t7, loop

addi $v0, $zero, 1
syscall
