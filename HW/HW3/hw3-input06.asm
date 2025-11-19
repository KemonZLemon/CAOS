addi $t1,$a1,-50 ###
VVVVV:beq $t1,$a2,DONE
sra $t8,$a3,3
or $v0,$s3,$t8
add $t1,$t1,$k0
j VVVVV
DONE:or $v1,$t8,$zero#surprise!
jr $a0
