#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../mylib.h"

#define NUM (1<<17)	
#define _16B (16)
#define _4MB (4*1024*1024)

//stress test

int main()
{
	unsigned long *p[NUM];
	unsigned long *ptr = 0;
	int ret = 0;

	for(int i = 0; i < NUM; i++)
	{
		p[i] = (unsigned long*)memalloc(_16B);
		if((p[i] == NULL) || (p[i] == (unsigned long*)-1))
		{
			printf("1.Testcase failed\n");
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
		//printf("Freeing chunk num: %d\n", i);
		ret = memfree(p[i]);
		if(ret != 0)
		{
			printf("2.Testcase failed\n");
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
		//printf("Freeing chunk num: %d\n", i);
		ret = memfree(p[i]);
		if(ret != 0)
		{
			printf("3.Testcase failed\n");
			return -1;
		}
	}

	//size of coalesced memory
	if(*(p[0]-1) != _4MB)
	{
		printf("4.Testcase failed\n");
		return -1;
	}

	printf("Testcase passed\n");
	return 0;
}


