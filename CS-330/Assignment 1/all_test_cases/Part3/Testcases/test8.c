#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../mylib.h"

//Request for memory is not a multiple of 8 bytes + 32 bytes extra memory in chunk remains after servicing the request 

int main()
{
	unsigned long *p1 = 0;
	unsigned long *p2 = 0;
	unsigned long *p3 = 0;
	unsigned long *p4 = 0;
	unsigned long *ptr = 0;
	int ret = 0;

	p1 = (unsigned long*)memalloc(16);
	if((p1 == NULL) || (p1 == (unsigned long*)-1))
	{
		printf("1.Testcase failed\n");
		return -1;
	}

	p2 = (unsigned long*)memalloc(16);
	if((p2 == NULL) || (p2 == (unsigned long*)-1))
	{
		printf("2.Testcase failed\n");
		return -1;
	}

	p3 = (unsigned long*)memalloc(56);
	if((p3 == NULL) || (p3 == (unsigned long*)-1))
	{
		printf("3.Testcase failed\n");
		return -1;
	}

	p4 = (unsigned long*)memalloc(32);
	if((p4 == NULL) || (p4 == (unsigned long*)-1))
	{
		printf("4.Testcase failed\n");
		return -1;
	}

	ret = memfree(p3);
	if(ret != 0)
	{
		printf("5.Testcase failed\n");
		return -1;
	}

	ret = memfree(p1);
	if(ret != 0)
	{
		printf("6.Testcase failed\n");
		return -1;
	}

	//Make a request which is not a multiple of 8 bytes
	ptr = (unsigned long*)memalloc(18);
	if((ptr == NULL) || (ptr == (unsigned long*)-1))
	{
		printf("7.Testcase failed\n");
		return -1;
	}

	//First fit
	if(ptr != p3)
	{
		printf("8.Testcase failed\n");
		return -1;
	}

	//chunk of 64 bytes is split into 2 parts of 32 bytes each
	//Checking the size of 32 bytes chunk allocated
	if(*(ptr-1) != 32)
	{
		printf("9.Testcase failed\n");
		return -1;
	}

	//Checking the size of 32 bytes chunk added to the free list and the split
	if(*(ptr+3) != 32)
	{
		printf("10.Testcase failed\n");
		return -1;
	}

	//inserted most recently free chunk at the head of the free list
	//next pointer of the 32 bytes free chunk (broken from 64 bytes chunk) should point to the p1
	if((unsigned long*)(*(ptr+4)) != (p1-1))
	{
		printf("11.Testcase failed\n");
		return -1;
	}

	printf("Testcase passed\n");
	return 0;
}


