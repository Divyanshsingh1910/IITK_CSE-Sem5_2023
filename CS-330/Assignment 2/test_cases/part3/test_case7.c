#include<ulib.h>

static int func4(int a, int b, int c, int d)
{
   return (a+b+c+d);
}

static int func3(int a, int b, int c)
{
   return (a+b+c);
}

static int func2(int a, int b)
{
   return (a+b);
}

static int func1(int a)
{
   return (2*a);
}

static int func0()
{
   return 0;
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
    
    ret = ftrace((unsigned long)&func0, ADD_FTRACE, 0, ftrace_fd);
    if(ret != 0)
    { 
	printf("1. Test case failed\n");
	return -1;
    }

    ret = ftrace((unsigned long)&func0, ENABLE_FTRACE, 0, ftrace_fd);
    if(ret != 0)
    { 
	printf("2. Test case failed\n");
	return -1;
    }
    
    ret = ftrace((unsigned long)&func1, ADD_FTRACE, 1, ftrace_fd);
    if(ret != 0)
    { 
	printf("3. Test case failed\n");
	return -1;
    }

    ret = ftrace((unsigned long)&func1, ENABLE_FTRACE, 0, ftrace_fd);
    if(ret != 0)
    { 
	printf("4. Test case failed\n");
	return -1;
    }
    
    ret = ftrace((unsigned long)&func2, ADD_FTRACE, 2, ftrace_fd);
    if(ret != 0)
    { 
	printf("5. Test case failed\n");
	return -1;
    }

    ret = ftrace((unsigned long)&func2, ENABLE_FTRACE, 0, ftrace_fd);
    if(ret != 0)
    { 
	printf("6. Test case failed\n");
	return -1;
    }
    
    ret = ftrace((unsigned long)&func3, ADD_FTRACE, 3, ftrace_fd);
    if(ret != 0)
    { 
	printf("7. Test case failed\n");
	return -1;
    }

    ret = ftrace((unsigned long)&func3, ENABLE_FTRACE, 3, ftrace_fd);
    if(ret != 0)
    { 
	printf("8. Test case failed\n");
	return -1;
    }
    
    ret = ftrace((unsigned long)&func4, ADD_FTRACE, 4, ftrace_fd);
    if(ret != 0)
    { 
	printf("9. Test case failed\n");
	return -1;
    }

    ret = ftrace((unsigned long)&func4, ENABLE_FTRACE, 4, ftrace_fd);
    if(ret != 0)
    { 
	printf("10. Test case failed\n");
	return -1;
    }

    func0();
    func1(1);
    func2(1,2);
    func3(1,2,3);
    func4(1,2,3,4);


    int read_ret = read_ftrace(ftrace_fd, ftrace_buff, 10);
    /*
    for(int i = 0; i<(read_ret/8); i++){
	printf("ftrace_buff[%d] : %x\n", i*8, ftrace_buff[i]);
    }*/
    
    if(read_ret != 120)
    { 
	printf("11. Test case failed\n");
	return -1;
    }

    if((u64*)(((u64*)ftrace_buff)[0]) != (u64*)(&func0))
    { 
	printf("12. Test case failed\n");
	return -1;
    }

    if((u64*)(((u64*)ftrace_buff)[1]) != (u64*)(&func1))
    { 
	printf("13. Test case failed\n");
	return -1;
    }

    if((u64*)(((u64*)ftrace_buff)[3]) != (u64*)(&func2))
    { 
	printf("14. Test case failed\n");
	return -1;
    }

    if((u64*)(((u64*)ftrace_buff)[6]) != (u64*)(&func3))
    { 
	printf("15. Test case failed\n");
	return -1;
    }

    if((u64*)(((u64*)ftrace_buff)[10]) != (u64*)(&func4))
    { 
	printf("16. Test case failed\n");
	return -1;
    }

    if(((u64*)ftrace_buff)[11] != 0x1)
    { 
	printf("17. Test case failed\n");
	return -1;
    }

    if(((u64*)ftrace_buff)[12] != 0x2)
    { 
	printf("18. Test case failed\n");
	return -1;
    }

    if(((u64*)ftrace_buff)[13] != 0x3)
    { 
	printf("19. Test case failed\n");
	return -1;
    }

    if(((u64*)ftrace_buff)[14] != 0x4)
    { 
	printf("20. Test case failed\n");
	return -1;
    }

    printf("Test case passed\n");

    return 0;
}
  
