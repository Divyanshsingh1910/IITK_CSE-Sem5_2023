#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

#define mb 1048576

unsigned long* head=NULL;

unsigned long* Check_left_cont(unsigned long *ptr){
	// printf("----------------->left loop CALLED!!\n");
	unsigned long* p = head;
	while(p && (p + (*p)/8) != ptr){
		p = (unsigned long*)*(p+1);
	}
	// printf("----------------->left loop ended!!\n");
	return p;
}

unsigned long* Check_right_cont(unsigned long *ptr){
	// printf("----------------->right loop CALLED!!\n");
	unsigned long* p = head;
	while(p && p != (ptr + (*ptr)/8)){
		p = (unsigned long*)*(p+1);
	}
	// printf("----------------->right loop ended!!\n");
	return p;	
}

unsigned long* get_pointer(unsigned long size){
	unsigned long* p = head;
	while(p && *p < size){
		p = (unsigned long*)*(p+1);
	}
	// // printf("Done here!\n p returned:= %p\n",p);
	return p;
}

void make_head(unsigned long* ptr){
	*(head+2) = (unsigned long)ptr;
	*(ptr+2) = 0;
	*(ptr+1) = (unsigned long)head;
	head = ptr;
}

// unsigned long* Left_cont_handle(unsigned long* ptr){
// 	unsigned long* leftptr = Check_left_cont(ptr);
// 	if(leftptr){
// 		*leftptr = *leftptr + *ptr; // size update
// 		unsigned long* prev = *(leftptr+2);
// 		unsigned long* next = *(leftptr+1);

// 		if(prev){
// 			*(prev+1) = (unsigned long)next;
// 		}
// 		if(next){
// 			*(next+2) = (unsigned long)prev;
// 		}
// 		return leftptr;
// 	}
// 	else return ptr;
// }

// unsigned long* Right_cont_handle(unsigned long* ptr){
// 	unsigned long* rightptr = Check_right_cont(ptr);
// 	if(rightptr){
// 		*ptr = *ptr + *rightptr;

// 		unsigned long* prev = *(rightptr+2);
// 		unsigned long* next = *(rightptr+1);
// 		if(prev){
// 			*(prev+1) = (unsigned long)next;
// 		}
// 		if(next){
// 			*(next+2) = (unsigned long)prev;
// 		}
// 		return ptr;

// 	}
// 	else return ptr;
// }


void *memalloc(unsigned long apparent_size) 
{
	// // printf("memalloc() called\n");
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
		// // printf("size2ask:= %lu\n",size2ask);
		ptr = mmap(NULL, size2ask, PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
		// // printf("Mmap gave memory at %p\n",ptr);
		head = ptr;
		*head = real_size;
		// // printf("Size of allocated memory = %lu\n",*head);

		head  = head + real_size/8; //head now points to start of next free memory
		// // printf("Ptr of next free memory = %p\n",head);

		*head = (unsigned long)(size2ask - real_size);
		*(head + 1) = 0;
		*(head + 2) = 0;

		// // printf("Size of left memory = %f\n",(float)(*head)/mb);
		return ptr+1;
	}
	else{
		// Memory was there
		// check if enough memory with current chunk else iterate till last chunk reached
		// even didn't get a free chunk even then, get one from mmap

		unsigned long* curr = get_pointer(real_size); //Null if not found;
		if(curr){
			// printf("\n       [Got the memory chunk with enough Mem!]\n\n");
			// we have the memory
			// curr+1 will be returned
			// now the left memory will be dealt based on the size of it

			unsigned long* prev = NULL; //prev free mem
			unsigned long* next = NULL; // next free mem

			if(*(curr+1)) next = (unsigned long*)*(curr+1);
			if(*(curr+2)) prev = (unsigned long*)*(curr+2);

			// printf(" Next Mem at %p\nPrevs Mem at %p\n",next,prev);

			if(*curr - real_size>=24){
			
				// b>=24 -> add the free memory to the start of head
				unsigned long* free_mem;
				free_mem = curr + real_size/8;
				*free_mem = *curr - real_size;
				*curr = real_size;
				// *(free_mem+2) = 0;
				// *(free_mem+1) = (unsigned long)head;
				// head = free_mem;
				// // printf("Checkpoint - 1\n");
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
					*(head + 2) = (unsigned long)free_mem;
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
				// // printf("Checkpoint - 2\n");
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
			
			// printf("\n     [Found No free chunk with enough Mem]\n\n");
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
	// printf("memfree() called\n");
	// ptr = Left_cont_handle(ptr);
	//---------
		unsigned long* leftptr = Check_left_cont(ptr);
		if(leftptr){
			*leftptr = *leftptr + *ptr; // size update
			unsigned long* prev = (unsigned long*)*(leftptr+2);
			unsigned long* next = (unsigned long*)*(leftptr+1);

			if(prev){
				*(prev+1) = (unsigned long)next;
			}
			if(next){
				*(next+2) = (unsigned long)prev;
			}
			ptr = leftptr;
		}
		// else return ptr;
	//---------
	// ptr = Right_cont_handle(ptr);
	// -----------
		unsigned long* rightptr = Check_right_cont(ptr);
		if(rightptr){
			*ptr = *ptr + *rightptr;

			unsigned long* prev = (unsigned long*)*(rightptr+2);
			unsigned long* next = (unsigned long*)*(rightptr+1);
			if(prev){
				*(prev+1) = (unsigned long)next;
			}
			if(next){
				*(next+2) = (unsigned long)prev;
			}
			// return ptr;

		}
		// else return ptr;
	// -----------
	make_head(ptr);
	return 0;
}

