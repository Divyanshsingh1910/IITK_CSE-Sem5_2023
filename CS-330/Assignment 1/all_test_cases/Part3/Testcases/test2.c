#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../mylib.h"

#define NUM (1<<17)	
#define _24B (24)
#define _1MB (1*1024*1024)
#define _3MB (3*1024*1024)
#define _4MB (4*1024*1024)

//Free list doesn't have any free chunk of requested size. Force the allocation of a new 4MB chunk.
int main()
{
	unsigned long *p[NUM];
	unsigned long *ptr = 0;
	int ret = 0;

	//allocate whole 4MB chunk
	for(int i = 0; i < NUM; i++)
	{
		p[i] = (unsigned long*)memalloc(_24B);
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
		ret = memfree(p[i]);
		if(ret != 0)
		{
			printf("2.Testcase failed\n");
			return -1;
		}
	}

	//allocate memory. Should be allocated from a new 4MB chunk
	ptr = (unsigned long*)memalloc(_1MB-8);
	if((ptr == NULL) || (ptr == (unsigned long*)-1))
	{
		printf("3.Testcase failed\n");
		return -1;
	}

	//size of free memory remaining in the new 4MB chunk after above allocation
	if(*(unsigned long*)((char*)ptr + _1MB - 8) != _3MB)
	{
		printf("4.Testcase failed\n");
		return -1;
	}

	printf("Testcase passed\n");
	return 0;
}


