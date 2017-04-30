addiu $s0, $zero, 0xffff
addiu $s1, $zero, 0xffff
mult $s0, $s1
mfhi $zero
mflo $s2
multu $s0, $s1
addi $s0, $zero, 65534
addi $s1, $zero, -2
mult $s0, $s1
multu $s0, $s1
mflo $zero
mflo $s2
halt
halt
halt
halt
halt
