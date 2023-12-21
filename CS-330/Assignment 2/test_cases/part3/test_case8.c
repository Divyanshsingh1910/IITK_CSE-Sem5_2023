#include<ulib.h>

static int func1(int a)
{
   return (2*a);
}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
    int ret = 0;
    int ftrace_fd = 0;
    u64 ftrace_buff[512];

    ftrace_fd = create_trace_buffer(O_RDWR);
    if(ftrace_fd != 3)
    {
	printf("0. Test case failed\n");
	return -1;
    }
    
    ret = ftrace((unsigned long)&func1, ENABLE_FTRACE, 1, ftrace_fd);
    if(ret != -EINVAL)
    { 
	printf("1. Test case failed\n");
	return -1;
    }
    
    printf("Test case passed\n");

    return 0;
}
  
