#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

#define mb 1048576

unsigned long* head=NULL;

void print(){
	printf("\n===========\nprinting free list\n===========\n\n"); 
	int i = 0;
	unsigned long *temp = head;
	while(temp != NULL){
		unsigned long size, prev, next;
		size = *temp;
		next = *(temp + 1);
		prev = *(temp + 2);
		printf("i = %d\naddress = %p\nsize = %lu\nprev = %x\nnext = %x\n\n", i++, temp, size, prev, next);
		temp = *(temp + 1);
	}
	printf("===========\n\n"); 
}

unsigned long* Check_left_cont(unsigned long *ptr){
	printf("----------------->left loop CALLED!!\n");
	unsigned long* p = head;
	while(p && (p + (*p)/8) != ptr){
		p = (unsigned long*)*(p+1);
	}
	printf("----------------->left loop ended!!\n");
	return p;
}

unsigned long* Check_right_cont(unsigned long *ptr){
	printf("----------------->right loop CALLED!!\n");
	unsigned long* p = head;
	while(p && p != (ptr + (*ptr)/8)){
		p = (unsigned long*)*(p+1);
	}
	printf("----------------->right loop ended!!\n");
	return p;	
}

unsigned long* get_pointer(unsigned long size){
	unsigned long* p = head;
	while(p && *p < size){
		p = (unsigned long*)*(p+1);
	}
	// printf("Done here!\n p returned:= %p\n",p);
	return p;
}
                                 
void *memalloc(unsigned long apparent_size) 
{
	printf("memalloc() called\n");
	if(apparent_size == 0){
		return NULL;
	} 
	if(apparent_size<16) apparent_size = 16;
	apparent_size += 8;
	unsigned long real_size = (apparent_size)%8 == 0 ? (apparent_size) : ((apparent_size)/8 + 1)*8;
	unsigned long size2ask = (real_size%(4*mb)==0) ? real_size : ((real_size)/(4*mb)+1)*4*mb; 
	if(head==NULL){
		// No memory allocated initially
		// will call mmap to get memory;
		unsigned long* ptr;
		printf("size2ask:= %lu\n",size2ask);
		ptr = mmap(NULL, size2ask, PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
		printf("Mmap gave memory at %p\n",ptr);
		head = ptr;
		*head = real_size;
		printf("Size of allocated memory = %lu\n",*head);

		head  = head + real_size/8; //head now points to start of next free memory
		printf("Ptr of next free memory = %p\n",head);

		*head = (unsigned long)(size2ask - real_size);
		*(head + 1) = 0;
		*(head + 2) = 0;

		printf("Size of left memory = %f\n",(float)(*head)/mb);
		return ptr+1;
	}
	else{
		// Memory was there
		// check if enough memory with current chunk else iterate till last chunk reached
		// even didn't get a free chunk even then, get one from mmap

		unsigned long* curr = get_pointer(real_size); //Null if not found;
		if(curr){
			printf("\n       [Got the memory chunk with enough Mem!]\n\n");
			// we have the memory
			// curr+1 will be returned
			// now the left memory will be dealt based on the size of it

			unsigned long* prev = NULL; //prev free mem
			unsigned long* next = NULL; // next free mem

			if(*(curr+1)) next = (unsigned long*)*(curr+1);
			if(*(curr+2)) prev = (unsigned long*)*(curr+2);

			printf(" Next Mem at %p\nPrevs Mem at %p\n",next,prev);

			if(*curr - real_size>=24){
			
				// b>=24 -> add the free memory to the start of head
				unsigned long* free_mem;
				free_mem = curr + real_size/8;
				*free_mem = *curr - real_size;
				*curr = real_size;
				// *(free_mem+2) = 0;
				// *(free_mem+1) = (unsigned long)head;
				// head = free_mem;
				// printf("Checkpoint - 1\n");
				//linking the prev and next free memory
				// <------------
				if(!prev && !next){
					// head == curr and head->next==NULL
					head = free_mem;
					*(head + 1) = 0;
					*(head + 2) = 0;
				}
				else if(!prev){
					head = free_mem;
					*(head+2) = 0;
				}
				else if(!next){
					*(prev+1) = 0;
					*(free_mem+2) = 0;
					*(free_mem+1) = (unsigned long) head;
					*(head + 2) = free_mem;
					head = free_mem;
				}
				else{
					*(prev+1) = (unsigned long)next;
					*(next+2) = (unsigned long)prev;
					*(free_mem+1) = (unsigned long) head;
					*(free_mem+2) = 0;
					head = free_mem;
				}
				// ------------>
				// printf("Checkpoint - 2\n");
			}
			else{
				// b<24 case

				if(!prev && !next){
					head==NULL;
				}
				else if(!prev){
					head = next;
					*(head+2)=0;
				}
				else if(!next){
					*(prev+1)=0;
				}
				else{
					*(prev+1) = (unsigned long)next;
					*(next+2) = (unsigned long)prev;
				}
			}

			return curr+1;

		}
		else{
			
			printf("\n     [Found No free chunk with enough Mem]\n\n");
			// we don't have the memory let's take it form mmap  
			unsigned long* ptr;

			//getting the memory from OS
			ptr = mmap(NULL, size2ask, PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);

			*ptr = real_size; //size of the allocated mem 

			unsigned long *next;
			next  = ptr + real_size/8; //next now points to start of next free memory

			*next = (unsigned long)(size2ask - real_size);
			*(next + 1) = (unsigned long) head; //next ptr
			*(next + 2) = 0;     //prev ptr
			*(head+2) = (unsigned long) next;

			head = next;         //updating the head

			return ptr+1;	
		}
	}
	
	return NULL;
}

int memfree(void *pr)
{	
	unsigned long* ptr = (unsigned long*)pr;
	ptr = ptr-1;
	printf("memfree() called\n");
	// check right and left cont free
	unsigned long* right = Check_right_cont(ptr);
	unsigned long* left = Check_left_cont(ptr);

	printf("--------------> LOOPS ENDED\n");

	printf(" right check at %p\nLeft check at %p\n",right,left);

	if(left && right){
		printf("dono mil gyein\n");
		printf("\n------------\n Case 0 \n----------\n");
		// dono side free mem hain
		*left = *left + (unsigned long)*ptr + *right;

		unsigned long* prev = NULL; //prev free mem
		unsigned long* next = NULL; // next free mem

		if(*(right+1)) next = (unsigned long*)*(right+1);
		if(*(left+2)) prev = (unsigned long*)*(left+2);

		printf(" Next Mem at %p\nPrevs Mem at %p\n",next,prev);

		if(!next && !prev){
			// left is head yehi bhramha hai
			head = left;
			*(head + 1) = 0;
			*(head + 2) = 0;
		}
		else if(!next){
			// sirf prev hai
			*(prev+1)=0;
			*(head+2) = (unsigned long)left;
			*(head + 1) = 0;
			*(left+2)=0; //prev of left
			*(left+1)=(unsigned long)head; //next of left
			head = left;
		}
		else if(!prev){
			// sirf next hai
			// phir wahi mtlb ki left head hai
			*(next+2) = (unsigned long) left;
			*(left + 2) = 0;
			head = left;
		}
		else{
			// dono hi side hain
			*(prev+1) =(unsigned long) next;
			*(next+2) = (unsigned long) prev;
			*(left+2)=0; //prev of left
			*(head+2) = (unsigned long)left;
			*(left+1)=(unsigned long)head; //next of left
			head = left;
		}
	}
	else if(left){
		printf("left mila bas\n");
		printf("\n------------\n Case 1 \n----------\n");
		// sirf left mein free mem hain
		*left = *left + *ptr;
		unsigned long* prev = NULL; //prev free mem
		unsigned long* next = NULL; // next free mem

		if(*(left+1)) next = (unsigned long*)*(left+1);
		if(*(left+2)) prev = (unsigned long*)*(left+2);

		printf(" Next Mem at %p\nPrevs Mem at %p\n",next,prev);

		if(!next && !prev){
			// left is head yehi bhramha hai
			head = left;
			*(head + 1) = 0;
			*(head + 2) = 0;
		}
		else if(!next){
			// prev hai bas
			*(prev+1)=0;
			*(left+2)=0; //prev of left
			*(head+2)=(unsigned long)left;
			*(left+1)=(unsigned long)head; //next of left
			head = left;
		}
		else if(!prev){
			// next hai bas
			head = left;
			*(head + 2) = 0;
		}
		else{
			// dono hi side null nhi hai 
			*(prev+1) =(unsigned long) next;
			*(next+2) = (unsigned long) prev;
			*(left+2)=0; //prev of left
			*(head+2) = (unsigned long)left;
			*(left+1)=(unsigned long)head; //next of left
			head = left;
		}
	}
	else if(right){
		printf("right mila bas\n");
		printf("\n------------\n Case 2 \n----------\n");
		// sirf right mein free mem hain
		*ptr = *ptr + *right;
		*(ptr+1) = *(right+1);
		*(ptr+2) = *(right+2);
		right = ptr;

		unsigned long* prev = NULL; //prev free mem
		unsigned long* next = NULL; // next free mem

		if(*(right+1)) next = (unsigned long*)*(right+1);
		if(*(right+2)) prev = (unsigned long*)*(right+2);

		printf(" Next Mem at %p\nPrevs Mem at %p\n",next,prev);

		if(!next && !prev){
			// right is head yehi bhramha hai
			head = right;
		}
		else if(!next){
			// next null hai but not prev
			*(prev+1)=0;
			*(right+2)=0; //prev of left
			*(head+2) = (unsigned long)right;
			*(right+1)= (unsigned long)head; //next of left
			head = right;
		}
		else if(!prev){
			// prev null hai but not next
			// phir wahi mtlb ki left head hai
			head = right;
		}
		else{
			// dono hi side hain
			*(prev+1) =(unsigned long) next;
			*(next+2) = (unsigned long) prev;
			*(right+2)=0; //prev of left
			*(head+2) = (unsigned long)right;
			*(right+1)=(unsigned long)head; //next of left
			head = right;
		}
	}
	else{
		printf("\n------------\n Case 3 \n----------\n");
		// dono hi side free nhi hain
		*(ptr+2) = 0;
		*(head+2) = (unsigned long)ptr;
		*(ptr+1) = (unsigned long) head;
		head = ptr;
	}
	return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM 3
#define _1GB (1024*1024*1024)

void f(){
printf("\n===========\nprinting free list\n===========\n\n"); 
unsigned long *temp = head;
	unsigned long size, prev, next;
	int i = 0;
	size = *temp;
	next = *(temp + 1);
	prev = *(temp + 2);
	printf("i = %d\naddress = %p\nsize = %lu\nprev = %x\nnext = %x\n\n", i++, temp, size, prev, next);
	temp = *(temp + 1);
	printf("===========\n\n"); 
	printf("\n===========\nprinting free list\n===========\n\n"); 
	size = *temp;
	next = *(temp + 1);
	prev = *(temp + 2);
	printf("i = %d\naddress = %p\nsize = %lu\nprev = %x\nnext = %x\n\n", i++, temp, size, prev, next);
	temp = *(temp + 1);
	printf("===========\n\n"); 
}

//Handling large allocations
int main()
{
	char *p[NUM];
	char *q = 0;
	int ret = 0;
	int a = 0;

	for(int i = 0; i < NUM; i++)
	{
		p[i] = (char*)memalloc(_1GB);
		if((p[i] == NULL) || (p[i] == (char*)-1))
		{
			printf("1.Testcase failed\n");
			return -1;
		}
		printf("\n\np[%d] = %p\n\n", i, p[i]);
		print();
		for(int j = 0; j < _1GB; j++)
		{
			p[i][j] = 'a';
		}
	}
	
	// for(int i = 0; i < NUM; i++)
	// {
	// 	ret = memfree(p[i]);
	// 	print();
	// 	if(ret != 0)
	// 	{
	// 		printf("2.Testcase failed\n");
	// 		return -1;
	// 	}
	// }
	memfree(p[0]);
	f();
	memfree(p[1]);
	f();
	printf("Testcase passed\n");
	return 0;
}