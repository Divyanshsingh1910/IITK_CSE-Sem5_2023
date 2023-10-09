#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../mylib.h"

#define NUM (1<<27)	
#define _24B (24)
#define _4GB (4*1024*1024*1024UL)

//stress test

int main()
{
	unsigned long **p = malloc(sizeof(unsigned long*) * NUM);
	unsigned long *ptr = 0;
	int ret = 0;

	ptr = (unsigned long*)memalloc(_4GB-8);
	if((ptr == NULL) || (ptr == (unsigned long*)-1))
	{
		printf("1.Testcase failed\n");
		return -1;
	}

	ret = memfree(ptr);
	if(ret != 0)
	{
		printf("2.Testcase failed\n");
		return -1;
	}

	for(int i = 0; i < NUM; i++)
	{
		p[i] = (unsigned long*)memalloc(_24B);
		if((p[i] == NULL) || (p[i] == (unsigned long*)-1))
		{
			printf("3.Testcase failed\n");
			return -1;
		}
	}

	//free every odd chunk
	for(int i = 0; i < NUM; i++)
	{
		if(i%2 == 0)
		{
			continue;
		}
		//printf("Freeing mem chunk at index: %d\n", i);	
		ret = memfree(p[i]);
		if(ret != 0)
		{
			printf("4.Testcase failed\n");
			return -1;
		}
	}

	//free every even chunk
	for(int i = 0; i < NUM; i++)
	{
		if(i%2 != 0)
		{
			continue;
		}
		//printf("Freeing mem chunk at index: %d\n", i);	
		ret = memfree(p[i]);
		if(ret != 0)
		{
			printf("5.Testcase failed\n");
			return -1;
		}
	}

	//size of coalesced memory
	if(*(p[0]-1) != _4GB)
	{
		printf("6.Testcase failed\n");
		return -1;
	}

	printf("Testcase passed\n");
	return 0;
}


