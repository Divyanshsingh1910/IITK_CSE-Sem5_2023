#include <stdio.h>
#include <unistd.h>
#include "../mylib.h"

//check metadata is maintained properly and allocation happens from correct location

int main()
{
	char *p = 0;
	char *q = 0;
	unsigned long size = 0;
	
	p = (char *)memalloc(1);
	if(p == NULL)
	{
		printf("1.Testcase failed\n");
		return -1;
	}

	q = (char *)memalloc(9);
	if(q == NULL)
	{
		printf("2.Testcase failed\n");
		return -1;
	}

	if(q != p+24)
	{
		printf("3.Testcase failed\n");
		return -1;
	}


	size = *((unsigned long*)q - 1);
	if(size != 24)
	{
		printf("4.Testcase failed\n");
		return -1;
	}


	printf("Testcase passed\n");
	return 0;
}


