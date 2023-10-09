#include <stdio.h>
#include <unistd.h>
#include "../mylib.h"

//first fit approach + freed memory chunk should be inserted at the head of the free list
//check metadata is maintained properly
int main()
{
	char *p1 = 0;
	char *p2 = 0;
	char *p3 = 0;
	char *p4 = 0;
	char *next = 0;
	int ret = 0;

	p1 = (char *)memalloc(16);
	if((p1 == NULL) || (p1 == (void*)-1))
	{
		printf("1.Testcase failed\n");
		return -1;
	}

	p2 = (char *)memalloc(16);
	if((p2 == NULL) || (p2 == (void*)-1))
	{
		printf("2.Testcase failed\n");
		return -1;
	}

	p3 = (char *)memalloc(16);
	if((p3 == NULL) || (p3 == (void*)-1))
	{
		printf("3.Testcase failed\n");
		return -1;
	}

	ret = memfree(p2);
	if(ret != 0)
	{
		printf("4.Testcase failed\n");
		return -1;
	}

	p4 = (char *)memalloc(16);
	if((p4 == NULL) || (p4 == (void*)-1))
	{
		printf("5.Testcase failed\n");
		return -1;
	}

	//first fit approach 
	if(p2 != p4)
	{
		printf("6.Testcase failed\n");
		return -1;
	}

	//verify that metadata (next pointer) in the memory chunk which was prev. free(p4) was maintained properly
	if(((char*)(*((unsigned long*)p4))) != (p3+16))	
	{
		printf("7.Testcase failed\n");
		return -1;
	}
	
	printf("Testcase passed\n");

	return 0;
}


