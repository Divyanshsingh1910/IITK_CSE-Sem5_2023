#include<ulib.h>

int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {

        int fd = create_trace_buffer(O_RDWR);
	char buff[10];
	char buff2[10];

	for(int i = 0; i< 10; i++){
		buff[i] = 'A' + i;
	}

	int ret = write(fd, buff, 10);
//	printf("ret value from write: %d\n", ret);
	if(ret != 10){
		printf("1.Test case failed\n");
		return -1;
	}

	int ret2 = read(fd, buff2, 10);
//	printf("ret value from write: %d\n", ret2);
	if(ret2 != 10){
		printf("2.Test case failed\n");
		return -1;	
	}
	int ret3 = ustrncmp(buff, buff2, 10);
	if(ret3 != 0){
		printf("3.Test case failed\n");
		return -1;	
	}
        close(fd);
	printf("Test case passed\n");
	return 0;
}
