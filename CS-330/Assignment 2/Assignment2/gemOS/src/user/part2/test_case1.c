#include<ulib.h>

int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {

        int strace_fd = create_trace_buffer(O_RDWR);
        int rdwr_fd = create_trace_buffer(O_RDWR);
	u64 strace_buff[4096];
	int read_buff[4096];
	
	start_strace(strace_fd, FULL_TRACING);
	int read_ret = read(rdwr_fd, read_buff, 10);
	if(read_ret != 0){
                printf("1.Test case failed\n");
                return -1;
        }
	end_strace();
	
	int strace_ret = read_strace(strace_fd, strace_buff, 2);
	if(strace_ret != 32){
		printf("2.Test case failed\n");
		return -1;
	}
	if(strace_buff[0] != 24){
		printf("3.Test case failed\n");
		return -1;
	}
	if((u64*)(strace_buff[2]) != (u64*)&read_buff){
		printf("4.Test case failed\n");
                return -1;
	}

        close(rdwr_fd);
        close(strace_fd);

	printf("Test case passed\n");
        return 0;
}
