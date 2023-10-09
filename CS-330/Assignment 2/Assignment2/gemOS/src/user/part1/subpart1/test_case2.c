#include<ulib.h>

int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {
        
	int fd = create_trace_buffer(O_READ | O_WRITE);
        char buff[4146];
        char buff2[4146];

	for(int i = 0, j = 0; i< 4146; i++){
                j = i % 26;
                buff[i] = 'A' + j;
        }

        int ret = write(fd, buff, 4096);
        if(ret != 4096){
                printf("1.Test case failed\n");
                return -1;
        }

	int ret2 = read(fd, buff2, 100);
        if(ret2 != 100){
                printf("2.Test case failed\n");
                return -1;
        }

	int ret3 = write(fd, buff+4096, 50);
        if(ret3 != 50){
                printf("3.Test case failed\n");
                return -1;
        }

        int ret4 = read(fd, buff2+100, 4046);
        if(ret4 != 4046){
                printf("4.Test case failed\n");
                return -1;
        }
        if(ustrncmp(buff, buff2, 4146)){
                printf("5.Test case failed\n");
                return -1;
        }
        printf("Test case passed\n");
        return 0;
}
