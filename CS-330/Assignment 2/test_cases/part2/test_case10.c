#include<ulib.h>

int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {

	int fork_ret = fork();
	if(fork_ret == 0){
		int strace_fd = create_trace_buffer(O_RDWR);
		if(strace_fd != 3){
			printf("1. Test case failed\n");
			return -1;
		}
		int rdwr_fd = create_trace_buffer(O_RDWR);
		if(rdwr_fd != 4){
			printf("2. Test case failed\n");
			return -1;
		}
		u64 strace_buff[4096];
		int read_buff[4096];
		int write_buff[4096];

		for(int i = 0, j = 0; i< 4096; i++){
			j = i % 26;
			write_buff[i] = 'A' + j;
		}
		int ret = strace(25, 0);
		if(ret < 0){
			printf("3. Test case failed\n");
			return -1;
		}
		ret = strace(2, 0);
		if(ret < 0){
			printf("4. Test case failed\n");
			return -1;
		}

		start_strace(strace_fd, FILTERED_TRACING);
		int write_ret = write(rdwr_fd, write_buff, 10);
		if(write_ret != 10){
			printf("5. Test case failed\n");
			return -1;
		}
		int read_ret = read(rdwr_fd, read_buff, 10);
		if(read_ret != 10){
			printf("6. Test case failed\n");
			return -1;
		}
		sleep(10);
		getpid();
		end_strace();

		int strace_ret = read_strace(strace_fd, strace_buff, 5);
		if(strace_ret != 40){
			printf("7. Test case failed\n");
			return -1;
		}
		if(strace_buff[0] != 25){
			printf("8. Test case failed\n");
			return -1;
		}
		if(strace_buff[2] != (u64)&write_buff){
                        printf("9. Test case failed\n");
                        return -1;
                }


		if(strace_buff[4] != 2){
			printf("10. Test case failed\n");
			return -1;
		}
		printf("child: Test case passed\n");
		close(strace_fd);
		close(rdwr_fd);	
	}
	else{
		int strace_fd = create_trace_buffer(O_RDWR);
                if(strace_fd != 3){
                        printf("11. Test case failed\n");
                        return -1;
                }
                int rdwr_fd = create_trace_buffer(O_RDWR);
                if(rdwr_fd != 4){
                        printf("12. Test case failed\n");
                        return -1;
                }
                u64 strace_buff[4096];
                int read_buff[4096];
                int write_buff[4096];

                for(int i = 0, j = 0; i< 4096; i++){
                        j = i % 26;
                        write_buff[i] = 'A' + j;
                }
                int ret = strace(25, 0);
                if(ret < 0){
                        printf("13. Test case failed\n");
			return -1;
                }
                ret = strace(2, 0);
                if(ret < 0){
                        printf("14. Test case failed\n");
			return -1;
                }

                start_strace(strace_fd, FILTERED_TRACING);
                int write_ret = write(rdwr_fd, write_buff, 10);
                if(write_ret != 10){
                        printf("15. Test case failed\n");
                        return -1;
                }
		int read_ret = read(rdwr_fd, read_buff, 10);
                if(read_ret != 10){
                        printf("16. Test case failed\n");
                        return -1;
                }
                sleep(10);
                getpid();
                end_strace();

                int strace_ret = read_strace(strace_fd, strace_buff, 5);
                if(strace_ret != 40){
                        printf("17. Test case failed\n");
                        return -1;
                }
                if(strace_buff[0] != 25){
                        printf("18. Test case failed\n");
                        return -1;
                }
		if(strace_buff[2] != (u64)&write_buff){
                        printf("19. Test case failed\n");
                        return -1;
                }


                if(strace_buff[4] != 2){
                        printf("20. Test case failed\n");
                        return -1;
                }
                printf("parent: Test case passed\n");
                close(strace_fd);
                close(rdwr_fd);

	}


}
