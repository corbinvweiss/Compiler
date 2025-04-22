const char* MALLOC_HEADER = R"(

	ferror:	.asciiz "Memory already free!\n"
	uerror:	.asciiz "Trying to free memory not malloced!\n"
	mused:  .word 0         # starting points for malloc
	mfree:  .word 0         # one ptr to memory list,
		                # one to the free list
)";

const char* MALLOC_BODY = R"(
	
####################################################
# malloc()
# Input:	$a0 holds number of bytes requested
# Returns:	$v0 holds the address of the chunk
#		$v1 holds the number of bytes allocated
# Description:	
#	  First we look through the free list to
#	see if we can find a free chunk of memory
#	that is the same size as we want to
#	allocate.  If we find one, we will take it
#	out of the free list and add it to the
#	used list.  If we don't find one, we will
#	call sbrk() and add the newly created chunk
#	to the used list
#         Malloc will add 4 bytes to the amount of
#       memory allocated to use as the reference
#       counter for instances of decaf classes.
####################################################
	
malloc:
	     ##############################################
	sw $fp ($sp)	# set up the stack - save $fp and $ra
	sw $ra -4($sp)
	sub $sp $sp 8
	     ##############################################

	add $a0 $a0 4	# add 4 bytes of data for the reference counter
	move $v1 $a0	# move size of memory requested into $a1
	lw $t0 mfree	# load the pointer in mfree
	        ##### while loop to find freed memory #####
loop1:
	beqz $t0 sbrk	# if the next free node is NULL, branch to sbrk
	lw $t1 ($t0)	# load size of first list
	bgt $t1 $v1 sbrk# if next free node is too big, we need to call sbrk
	bne $t1 $v1 cont1 # if sizes not equal, continue while loop1
	lw $v0 4($t0)	# load the first free block into $v0
	beqz $v0 sbrk	# if there is no free block of this size go to sbrk
	lw $t2 4($v0)	# load next block
	beqz $t2 nonext	# if next==NULL
	sw $zero 8($t2)	# store NULL in next->prev
nonext:
	sw $t2 4($t0)	# store next address in the free list
	b created
                ##### get next node to continue the while loop #####
cont1:
	lw $t0 8($t0)	# get address of next node
	b loop1		# continue the loop1
		##### add some memory to the heap #####
sbrk:
	add $a0 $a0 16	# add 16 bytes overhead
	li $v0 9	# syscall 9 (sbrk)
	syscall		#
		##### chunk of memory set up, so set up the pointers #####
created:
	sw $v1 ($v0)	# put the number of bytes into the struct
	lw $t2 mused	# load addr of mused into $t2
	sw $t2 4($v0)	# point next to mused
	sw $v0 mused	# point mused to this memory
	sw $zero 8($v0)	# make prev -> NULL
	li $t3 1	# type 1 = used
	sw $t3 12($v0)	# make type = used (1)
	add $v0 $v0 16	# return the pointer to the new memory
	
	     ##############################################
	lw $ra 4($sp)	# restore the stack
	lw $fp 8($sp)
	add $sp $sp 8
	jr $ra
	     ##############################################


#############################################
# free()
# Input:	$a0 holds addr to free
# Description:	
#	  When a chunk of memory is freed,
#	it is moved from the used list to
#	the free list.
#         If the memory chunk is already
#	free, then we will abort with an
#	error message.
#	  If the memory we are trying to free
#	has not been malloced, then we print
#	out an error and abort.
#
#		!!!!!WARNING!!!!!
#
#	  The error checking done here may
#	help you detect memory errors with
#	your compiler.  While this may make
#	it very worthwhile, it will slow
#	down free() quite a bit.  Be warned
#	that once your compiler is working,
#	you should probably remove the error
#	checking stuff because your code
#	that is automatically generated
#	MUST NOT contain memory errors, so
#       you shouldn't need to check for them.
#############################################

free:
	     ##############################################
	sw $fp ($sp)	# set up the stack - save $fp and $ra
	sw $ra -4($sp)
	sub $sp $sp 8
	     ##############################################

	sub $t1 $a0 16	# subtract 16 from the address to get the start
			# of the malloc overhead for this chunk
	
	     ##### START OF ERROR CHECKING CODE #####
	lw $t0 -4($a0)	# load memory type
	bnez $t0 goodtype # if memory is used, continue
	la $a0 ferror	# otherwise, trying to free already free memory
	li $v0 4	#
	syscall		# print error message
	li $v0 10	#
	syscall		# exit program
goodtype:
	lw $t0 mused	# load address of first used block
usedloop:
	beqz $t0 usederror # memory not malloced
	beq $t0 $t1 goodaddress # memory address is valid
	lw $t0 4($t0)	# load next address in used list
	b usedloop	# while (ptr!=NULL)
usederror:
	la $a0 uerror	# otherwise, trying to free memory not malloced
	li $v0 4	#
	syscall		# print error message
	li $v0 10	#
	syscall		# exit program
	     ##### END OF ERROR CHECKING CODE #####
	
	     ##### at this point we know we were passed a good address #####
goodaddress:
	lw $t0 mfree	# load the address of the free list
	move $t7 $t1	# move address of memory chunk to $t7
	lw $t2 ($t1)	# load number of bytes in memory chunk
	bnez $t0 exists	# if mfree != NULL branch to else1
	     ##### there are no free chunks yet #####
	li $a0 16	# need 16 bytes for a new free list node
	li $v0 9	# syscall 9 (sbrk)
	syscall
	move $t3 $v0	# move address of new node into $t3
	sw $t3 mfree	# change mfree to this new node
	sw $t2 ($t3)	# store the number of bytes in the memory chunk
	sw $t7 4($t3)	# store address of memory in free node
	sw $zero 8($t3)	# set next pointer = NULL
	sw $zero 12($t3)# set prev pointer = NULL
	b freeret	# return
	     ##### if there are free chunks in free list #####
	     ##### $t0=next free node  $t7=chunk being freed  $2=size #####
exists:
	lw $t3 ($t0)	# load number of bytes in list into $t3
	bge $t2 $t3 else1 # if chunk being freed is bigger than first free one
	li $a0 16	# need 16 bytes for a new free list node
	li $v0 9	# syscall 9 (sbrk)
	syscall		#
	sw $t2 ($v0)	# store number of bytes in chunk in free node
	sw $t7 4($v0)	# put address of free chunk in list pointer
	sw $t0 8($v0)	# store next free node
	sw $zero 12($v0)# prev free node = NULL
	sw $v0 mfree	# make first free node be the current node	
	b freeret	# return
else1:	
	lw $t3 ($t0)	# load number of bytes in list into $t3
	beq $t3 $t2 sizeeq # if size of list and size of chunk are equal
	lw $t4 8($t0)	# load next free node into $t4
	beqz $t4 next	# next pointer is NULL
	lw $t5 ($t4)	# load size of next node into $t5
	blt $t2 $t5 next# if chunk->size > next->size
	move $t0 $t4	# move next pointer into $t3
	b else1		# continue while loop	
	     ##### if node->size == chunk->size (we found it) #####
sizeeq:
	lw $t4 4($t0)	# load address of first chunk in list into $t4
	beqz $t4 cont2	# if list == NULL branch to cont2
	sw $t7 8($t4)	# store the freed block in the first block in list
	     ##### set up pointers for the list and freed chunk #####
cont2:	
	sw $t7 4($t0)	# node->list  chunk
	sw $t4 4($t7)	# chunk->next = node->list
	sw $zero 12($t7)# type == 0 (free)
	b freeret	# return from free()
	     ##### if node->size < chunk->size (no proper freelist here) #####
	     ##### $t0 = ptr before new   $t4 = ptr after new #####
next:
	li $a0 16	# need 16 bytes for a new free list node
	li $v0 9	# syscall 9 (sbrk)
	syscall		#
	sw $t2 ($v0)	# store number of bytes in chunk in free node
	sw $t7 4($v0)	# put address of free chunk in list pointer
	sw $t4 8($v0)	# set next->node to the next free node
	sw $t0 12($v0)	# store address of prev free node
	beqz $t4 cont3	# if node->next == NULL no next node to change pointers
	sw $t7 12($t4)	# node->next->prev=node
cont3:	
	sw $v0 8($t0)	# store new free node in next pointer
	b freeret	# return

freeret:	
	lw $t0 4($t7)
	lw $t1 8($t7)
	beqz $t0 fr2
	sw $t1 8($t0)
fr2:
	beqz $t1 fr3
	sw $t0 4($t1)
fr3:
	     ##############################################
	lw $ra 4($sp)	# restore the stack
	lw $fp 8($sp)
	add $sp $sp 8
	jr $ra
	     ##############################################
)";