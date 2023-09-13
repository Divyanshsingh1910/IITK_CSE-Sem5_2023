#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MB 1048576

unsigned long *head = NULL;

unsigned long next_greater_multiple_of_8_Bytes(unsigned long size){
	double result = size;
	result = result/8;
	result = ceil(result);
	result = result*8;
	return (unsigned long)result;
}

unsigned long next_greater_multiple_of_4_MB(unsigned long size){
	double result = size;
	result = result/(4*MB);
	result = ceil(result);
	result = result*4*MB;
	return (unsigned long)result;
}

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

unsigned long *find_free_chunk(unsigned long size){
	unsigned long *temp = head;
	while(temp != NULL && *temp < size){
		temp = *(temp + 1);
	}
	return temp;
}

void add_to_start(unsigned long *ptr){
	if(head != NULL){
		*(ptr + 1) = head;
		*(head + 2) = ptr;
	}
	head = ptr;
	print();
}

void delete_chunk(unsigned long *ptr){
	if(ptr == head){
		head = *(head + 1);
		if(head != NULL){
			*(head + 2) = 0;
		}
	}
	else{
		unsigned long *next, *prev;
		next = *(ptr + 1);
		prev = *(ptr + 2);
		*(prev + 1) = next;
		if(next != NULL){
			*(next + 2) = prev;
		}
	}
}

unsigned long *merge_left(unsigned long *ptr){
	unsigned long *temp;
	temp = head;
	unsigned long neighbour_size = *temp;
	while(temp != NULL && temp + neighbour_size/8 != ptr){
		temp = *(temp + 1);
		if(temp != NULL){
			neighbour_size = *temp;
		}
	}
	if(temp == NULL){
		printf("no left neighbour found\n");
		return ptr;
	}
	else{
		printf("left neighbour found\nneighbour address = %p\n", temp);
		unsigned long chunk_size;
		chunk_size = *ptr;
		unsigned long neighbour_size = *temp;
		neighbour_size = neighbour_size + chunk_size;
		*temp = neighbour_size;
		delete_chunk(temp);
		*(temp + 1) = 0;
		*(temp + 2) = 0;
		return temp;
	}
}

void *merge_right(unsigned long *ptr){
	unsigned long *right_boundary, *temp;
	unsigned long chunk_size;
	chunk_size = *ptr;
	right_boundary = ptr + chunk_size/8;
	temp = head;
	while(temp != NULL && temp != right_boundary){
		temp = *(temp + 1);
	}
	if(temp == NULL){
		printf("no right neighbour found\n");
	}
	else{
		printf("right neighbour found\nneighbour address = %p\n", temp);
		unsigned long neighbour_size = *temp;
		printf("neighbour size = %lu\n", neighbour_size);
		chunk_size = chunk_size + neighbour_size;
		*ptr = chunk_size;
		printf("chunk size = %lu\n", *ptr);
		delete_chunk(temp);
		*(temp + 1) = 0;
		*(temp + 2) = 0;
	}
}

void *memalloc(unsigned long size)
{
	printf("memalloc() called\nrequested size = %lu\n", size);
	if(size == 0){
		return NULL;
	}
	size += 8;
	if(size < 24){
		size = 24;
	}
	unsigned long *ptr = find_free_chunk(size);
	if(ptr == NULL){
		printf("no free chunk found\ncalling mmap to allocate new chunk\n");
		unsigned long size_requested_from_OS = next_greater_multiple_of_4_MB(size);
		printf("size requested from OS = %lu MB\n", size_requested_from_OS/MB);
		ptr = mmap(NULL, size_requested_from_OS, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
		if(ptr == MAP_FAILED){
			printf("mmap failed\n");
			return NULL;
		}
		printf("address of new chunk obtained from OS = %p\n", ptr);
		*ptr = size_requested_from_OS;
	}
	else{
		printf("free chunk found\nsize of free chunk found = %lu\nremoving the chunk from free list\n", *ptr);
		if(ptr == head){
			printf("head is the free chunk\n");
		}
		else{
			printf("head is not the free chunk\n");
		}
		delete_chunk(ptr);
	}
	unsigned long chunk_size = *ptr;
	size = next_greater_multiple_of_8_Bytes(size);
	*ptr = size;
	void *retptr = ptr + 1;
	printf("address of allocated chunk = %p\n", retptr);
	unsigned long b = chunk_size - size;
	printf("b = %lu\n", b);
	if(b >= 24){
		printf("b >= 24\nadding remaining chunk to start of free list\n");
		ptr = ptr + size/8;
		*ptr = b;
		*(ptr + 1) = NULL;
		*(ptr + 2) = NULL;
		add_to_start(ptr);
	}
	return retptr;
}

int memfree(void *_ptr)
{
	printf("memfree() called\naddress of _ptr = %p\n", _ptr);
	unsigned long *ptr = (unsigned long*)_ptr;
	ptr = ptr - 1;
	printf("ptr = %p\n", ptr);
	unsigned long size = *ptr;
	printf("size to be freed = %lu\n", size);
	ptr = merge_left(ptr);
	print();
	merge_right(ptr);
	print();
	*(ptr + 1) = NULL;
	*(ptr + 2) = NULL;
	add_to_start(ptr);
	return 0;
}