START:lui $t5,7777#test comment
lw $t6,-100($gp)
M:         div $t5,$t6
mfhi $v0
add $v0,$v0,$a2    #3
FIN: jr $a0
