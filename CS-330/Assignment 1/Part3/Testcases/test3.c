#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../mylib.h"

#define NUM 3
#define _1GB (1024*1024*1024)

//Handling large allocations
int main()
{
	char *p[NUM];
	char *q = 0;
	int ret = 0;
	int a = 0;

	for(int i = 0; i < NUM; i++)
	{
		p[i] = (char*)memalloc(_1GB);
		if((p[i] == NULL) || (p[i] == (char*)-1))
		{
			printf("1.Testcase failed\n");
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
			printf("2.Testcase failed\n");
			return -1;
		}
	}

	printf("Testcase passed\n");
	return 0;
}