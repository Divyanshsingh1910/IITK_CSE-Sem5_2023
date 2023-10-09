#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../mylib.h"

#define NUM (1000000)	
#define _4KB (4*1024)

//stress test

int main()
{
	unsigned long **p = malloc(sizeof(unsigned long*) * NUM);
	int *isFree = malloc(sizeof(int) * NUM);
	unsigned long *ptr = 0;
	int ret = 0;
	int size = 0;
	int pos = 0;

	srand(1);

	//allocate NUM chunks with size varying from 9 bytes to 4KB-1 bytes
	for(int i = 0; i < NUM; i++)
	{
		size = rand() % _4KB;
		if(size <= 8)
		{
			size = 9;
		}
		//printf("Allocating mem chunk of size %d\n", size);
		p[i] = (unsigned long*)memalloc(size);
		if((p[i] == NULL) || (p[i] == (unsigned long*)-1))
		{
			printf("1.Testcase failed\n");
			return -1;
		}
	}

	//initially, all NUM chunks are allocated
	for(int i = 0; i < NUM; i++)
	{
		isFree[i] = 0;
	}

	//free some of the allocated memory
	for(int i = 0; i < NUM; i++)
	{
		pos = rand() % NUM;
		if(isFree[pos] == 1)
		{
			//printf("Already Freed mem chunk at index %d. Skipping it.\n", pos);
			continue;
		}
		//printf("Freeing mem chunk at index %d\n", pos);
		ret = memfree(p[pos]);
		//printf("Freed mem chunk at index %d\n", pos);
		if(ret != 0)
		{
			printf("2.Testcase failed\n");
			return -1;
		}
		isFree[pos] = 1;
	}


	printf("Testcase passed\n");
	return 0;
}


