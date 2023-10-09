#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#include "../mylib.h"

#define _16MB (16 * 1024 * 1024)
#define _20MB (20 * 1024 * 1024)

//Check that the requested memory is serviced by the smallest multiple of 4MB that can satisfy the request

int main()
{
	unsigned long *p5 = 0;
	int ret = 0;
	
	//Should allocate 20MB chunk (16MB + metadata)	
	p5 = memalloc(_16MB);
	if((p5 == NULL) || (p5 == (unsigned long*)-1))
	{
		printf("1.Testcase failed\n");
		return -1;
	}

	ret = memfree(p5);
        if(ret != 0)
        {
                printf("2.Testcase failed\n");
                return -1;
        }


	//confirm that the chunk of memory which was allocated to service the request was 20MB (smallest multiple of 4MB able to satisfy the request)
	//printf("Chunk size: %lu\n", *(p5-1));
	if(*(p5-1) != (_20MB))
	{
		printf("3.Testcase failed\n");
		return -1;
	}


	printf("Testcase passed\n");
	return 0;
}


