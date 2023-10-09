#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../mylib.h"

#define NUM 6

//First fit + insertion of free memory chunk at the head of the free list

int main()
{
	unsigned long *p[NUM];
	unsigned long *ptr = 0;
	int size = 56;
	int ret = 0;

	for(int i = 0; i < NUM; i++)
	{
		p[i] = (unsigned long*)memalloc(size);
		if((p[i] == NULL) || (p[i] == (unsigned long*)-1))
		{
			printf("1.Testcase failed\n");
			return -1;
		}

		size = size - 8;
	}

	for(int i = 0; i < NUM; i++)
	{
		if(i%2 != 0)
		{
			continue;
		}
		ret = memfree(p[i]);
		if(ret != 0)
		{
			printf("2.Testcase failed\n");
			return -1;
		}
	}

	//Insertion of free memory chunk at the head of the free list
	//next of 56 bytes chunk should point to the free memory chunk after the 16 bytes chunk (p[5])
	if((unsigned long *)(*p[0]) != (p[5]+2))
	{
		printf("3.Testcase failed\n");
		return -1;
	}

	//next of 40 bytes chunk should point to the free 56 bytes chunk	
	if((unsigned long *)(*p[2]) != (p[0]-1))
	{
		printf("4.Testcase failed\n");
		return -1;
	}

	//prev of the 56 bytes chunk should point to the free 40 bytes chunk
	if((unsigned long *)(*(p[0]+1)) != (p[2]-1))
	{
		printf("5.Testcase failed\n");
		return -1;
	}


	//First fit
	ptr = (unsigned long*)memalloc(56);
	if((ptr == NULL) || (ptr == (unsigned long*)-1))
	{
		printf("6.Testcase failed\n");
		return -1;
	}

	if(ptr != p[0])
	{
		printf("7.Testcase failed\n");
		return -1;
	}

	printf("Testcase passed\n");
	return 0;
}


