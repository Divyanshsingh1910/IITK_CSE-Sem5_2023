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
}

void delete_chunk(unsigned long *ptr){
	if(ptr == head){
		head = *(head + 1);
		if(head != NULL){
			*(head + 2) = NULL;
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
		return ptr;
	}
	else{
		unsigned long chunk_size;
		chunk_size = *ptr;
		unsigned long neighbour_size = *temp;
		neighbour_size = neighbour_size + chunk_size;
		*temp = neighbour_size;
		delete_chunk(temp);
		*(temp + 1) = NULL;
		*(temp + 2) = NULL;
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
	if(temp != NULL){
		unsigned long neighbour_size = *temp;
		chunk_size = chunk_size + neighbour_size;
		*ptr = chunk_size;
		delete_chunk(temp);
		*(temp + 1) = NULL;
		*(temp + 2) = NULL;
	}
}

void *memalloc(unsigned long size)
{
	if(size == 0){
		return NULL;
	}
	size += 8;
	if(size < 24){
		size = 24;
	}
	unsigned long *ptr = find_free_chunk(size);
	if(ptr == NULL){
		unsigned long size_requested_from_OS = next_greater_multiple_of_4_MB(size);
		ptr = mmap(NULL, size_requested_from_OS, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
		if(ptr == MAP_FAILED){
			return NULL;
		}
		*ptr = size_requested_from_OS;
	}
	else{
		delete_chunk(ptr);
	}
	unsigned long chunk_size = *ptr;
	size = next_greater_multiple_of_8_Bytes(size);
	*ptr = size;
	void *retptr = ptr + 1;
	unsigned long b = chunk_size - size;
	if(b >= 24){
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
	unsigned long *ptr = (unsigned long*)_ptr;
	ptr = ptr - 1;
	unsigned long size = *ptr;
	ptr = merge_left(ptr);
	merge_right(ptr);
	*(ptr + 1) = NULL;
	*(ptr + 2) = NULL;
	add_to_start(ptr);
	return 0;
}