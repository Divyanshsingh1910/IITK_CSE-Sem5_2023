## Answers to the Quizzes

### Quiz 1

		Q1. Max: 4   Min: 1

		Q2. 0,1,2,3,4,5

		Q3. 12

		Q4. Yes

### Quiz 2

		Q1. Yes

		Q2. True

		Q3.  Both parent and child will print 0

		Q4. The child process will print "yes" as the ptr is not NULL in the child process (because of the successful brk call in line #9).
		
		The child process will further expand the BSS by 8192 bytes (at line # 15) and ptr will be p_start + 4096.  As a result, the child will print 4096 at line #18

### Quiz 3

		Q1.  (i) A, B, E
	         (ii) A, C, D, E
	
		Q2.
	
		(i) False. The OS may intervene if the address in RBX is invalid (OR not mapped in the translation tables).  In such a case, the hardware will raise a fault and the OS is required to handle that.
	
		(ii) False. If the kernel stack of the parent is not replicated during the execution of fork() , the saved user state of the parent process (at the time of fork) will not be copied to the child process. This may result in child not resuming at the same point as the parent and will break the fork() semantics.

### Quiz 4

		1. ABCDE
		2. ACE
		3. True/False
		4. True/False
**Note**: *Answer fo quiz 4 is not verified with instructor but most probably it is correct*

### Quiz 5
I actually the lost the actual answers, but I guess it won't be that hard to solve. Maybe someone can come up with answers & add here?🥺

### Quiz 6
		Q1.
		(i) {1 + (2 ^ 11) + (2 ^ 22)} * 16KB
		(ii) 2 ^ 25 = 32MB
		(iii) 0x4000000000 - 0x4FFFFFFFFF (4*2^36 - (5*2^36-1))
		(iv) 2 ^ 69 bytes
		(v) Min: 0 Max: (2 ^ 5 + 4) * 16KB 

### Quiz 7

		Q1. X belongs to S where S={x: x=100n and 10000 - 100K <= x <= 10000 + 100K, where n is an integer}
		Q2. Minimum = 1, Maximum = 4

### Quiz 8
		Q1. 
  		(i) 2^32 * 4KB = 16TB
	      	(ii) (8 + 4*1024 + 2*1024*1024) * 4KB = 8GB + 16MB + 32KB 
	      	(iii) three disk blocks (12KB)
	      	(iv) min: 4KB    max: 12KB

---
