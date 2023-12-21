#include<ulib.h>



int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {

        int fd = create_trace_buffer(O_RDWR);

	char* buff = (char*)expand(10, MAP_RD);
	if(buff == 0)
	{
		printf("Testcase unable to progress\n");
	}

	int ret = write(fd, buff, 10);
	if(ret != 10)
	{
		printf("1.Test case failed\n");
		return -1;
	}

	ret = read(fd, buff, 10);
	if(ret != -EBADMEM){
		printf("2.Test case failed\n");
		return -1;
	}

	printf("Test case passed\n");

	return 0;
}
