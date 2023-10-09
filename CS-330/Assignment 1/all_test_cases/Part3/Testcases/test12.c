#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../mylib.h"

#define NUM 6
#define _16B (16)

//free a memory chunk + coalesce the freed chunk with already free chunks on its left and right side

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

	//pointer to the remaining free memory in the 4MB chunk after above allocations
	ptr = p[5]+2;

	ret = memfree(p[4]);
	if(ret != 0)
	{
		printf("2.Testcase failed\n");
		return -1;
	}

	ret = memfree(p[0]);
	if(ret != 0)
	{
		printf("3.Testcase failed\n");
		return -1;
	}

	ret = memfree(p[2]);
	if(ret != 0)
	{
		printf("3.Testcase failed\n");
		return -1;
	}

	//should coalesce with p[2] and p[4]
	ret = memfree(p[3]);
	if(ret != 0)
	{
		printf("4.Testcase failed\n");
		return -1;
	}

	//size of coalesced memory
	if(*(p[2]-1) != 72)
	{
		printf("5.Testcase failed\n");
		return -1;
	}

	//free list should be manipulated properly after coalescing
	//next of p[2] should point to p[0]
	if(((unsigned long*)(*p[2])) != (p[0]-1))
	{
		printf("6.Testcase failed\n");
		return -1;
	}
	
	//next of p[0] should point to ptr
	if(((unsigned long*)(*p[0])) != (ptr))
	{
		printf("7.Testcase failed\n");
		return -1;
	}

	printf("Testcase passed\n");
	return 0;
}


