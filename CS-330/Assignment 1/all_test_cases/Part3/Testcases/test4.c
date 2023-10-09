#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../mylib.h"

#define NUM 4
#define _1GB (1024*1024*1024)
#define _4MB (4*1024*1024)
#define ONE_CHUNK_SIZE (_1GB + _4MB)

//Handling large allocations, coalescing
int main()
{
	char *p[NUM];
	char *q = 0;
	int ret = 0;
	int a = 0;

	
	q = (char*)memalloc((NUM * (unsigned long)ONE_CHUNK_SIZE) - 8);
	if((q == NULL) || (q == (char*)-1))
	{
		printf("1.Testcase failed\n");
		return -1;
	}
	ret = memfree(q);
	if(ret != 0)
	{
		printf("2.Testcase failed\n");
		return -1;
	}
	
	//printf("1.q: %p, size free: %lu. Expected: %lu\n", q, *((unsigned long*)q - 1), (NUM * (unsigned long)ONE_CHUNK_SIZE));

	for(int i = 0; i < NUM; i++)
	{
		p[i] = (char*)memalloc(_1GB);
		if((p[i] == NULL) || (p[i] == (char*)-1))
		{
			printf("3.Testcase failed\n");
			return -1;
		}

		for(int j = 0; j < _1GB; j++)
		{
			p[i][j] = 'a';
		}
	}

	for(int i = 0; i < NUM; i++)
	{
		ret = memfree(p[i]);
		if(ret != 0)
		{
			printf("4.Testcase failed\n");
			return -1;
		}
	}
	
	//printf("4.size after coalescing: %lu. Expected: %lu\n", *((unsigned long*)p[0] - 1), (NUM * (unsigned long)ONE_CHUNK_SIZE));
	if(*((unsigned long*)p[0] - 1) != (NUM * (unsigned long)ONE_CHUNK_SIZE))
	{
		printf("5.Testcase failed\n");
		return -1;
	}

	printf("Testcase passed\n");
	return 0;
}


