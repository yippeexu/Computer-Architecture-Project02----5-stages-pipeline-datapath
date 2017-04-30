	   addi	$a0, $zero, 0	# initial : int index = 0;
	   jal	tree			# call tree(0)
	   halt
	   halt
	   halt
	   halt
	   halt
	   halt
	   halt
	   halt
tree : addi	$sp, $sp, -8	# push two words (last layer)
	   sw	$ra, 4($sp)		# save previous return address
	   sw	$a0, 0($sp)		# save previous argument
	   addi	$t0, $0, 6		# t0 = 6
	   slt	$t1, $t0, $a0	# t1 = t0 < a0 ; ( a0 > 6 ) ?
	   beq	$t1, $zero, L1	# if (t1 == 0) { jump to L1 }

	   addi	$v0, $zero, 0	# else { return 0; }
	   addi	$sp, $sp, 8		# pop two words
	   jr	ra				# return

L1 :   sll	$s0, $a0, 2		# index * 4
	   lw	$s1, 0($s0)		# load d-mem ( s1 = d[4*i] )
	   addi	$a0, $a0, 1		# index++ ;
	   sll	$a0, $a0, 1		# index * 2
	   jal	tree			# call tree(2*i+2)
	   add	$s1, $s1, $v0	# s1 sum the return values up
	   addi	$a0, $a0, -1	# index-- ; (2*i+1)
	   jal	tree			# call tree(2*i+1)
	   add	$v0, $s1, $v0	# v0 sums the two return values

	   lw	$a0, 0($sp)		# load back the previous a0
	   lw	$ra, 4($sp)		# load back the previous ra
	   addi	$sp, $sp, 8		# pop two words
	   jr 	ra				# return