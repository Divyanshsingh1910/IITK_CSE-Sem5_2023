#include<ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5){
	int PAGE_SIZE = 4096;

	// validity check
	long res = mmap(NULL, 1, PROT_READ, 1812);
	if(res != -1){
		printf("validity check failed flags\n");
	}
	res = mmap(NULL, 1, 23, 0);
	if(res != -1){
		printf("validity check failed prot\n");
	}
	res = mmap(1, 1, PROT_READ, 0);
	if(res != -1){
		printf("validity check failed addr\n");
	}
	res = mmap(NULL, 1623749122, PROT_READ, 0);
	if(res != -1){
		printf("validity check failed length\n");
	}
	res = mmap(NULL, -1, PROT_READ, 0);
	if(res != -1){
		printf("validity check failed length\n");
	}
	
	int err = 1;
	
	char *ptr = mmap(NULL, PAGE_SIZE*1, PROT_READ, 0);
	if((long) ptr == -1){
		printf("%d tc failed\n", err++);
	}
	pmap(1);
	
	char *ptr2 = mmap(NULL, PAGE_SIZE*12, PROT_READ, 0);
	if((long) ptr2 == -1 || ptr2 != ptr + PAGE_SIZE){
		printf("%d tc failed\n", err++);
	}
	pmap(1);
	
	char *ptr3 = mmap(ptr + 25*PAGE_SIZE, PAGE_SIZE*12, PROT_READ, 0);
	if((long) ptr3 == -1 || ptr3 != ptr + 25*PAGE_SIZE){
		printf("%d tc failed\n", err++);
	}
	pmap(1);

    char *ptr4 = mmap(NULL, PAGE_SIZE*1, PROT_READ | PROT_WRITE, 0);
    if((long) ptr4 == -1 || ptr4 != ptr + 13*PAGE_SIZE){
		printf("%d tc failed\n", err++);
	}
	pmap(1);

    char *ptr5 = mmap(ptr + 15*PAGE_SIZE, PAGE_SIZE*1, PROT_READ, 0);
	if((long) ptr3 == -1 || ptr3 != ptr + 25*PAGE_SIZE){
		printf("%d tc failed\n", err++);
	}
	pmap(1);

	mprotect(ptr+2*PAGE_SIZE, 3*PAGE_SIZE, PROT_READ|PROT_WRITE);
	pmap(1);
	mprotect(ptr+5*PAGE_SIZE, PAGE_SIZE, PROT_READ|PROT_WRITE);
	pmap(1);
	mprotect(ptr+3*PAGE_SIZE, PAGE_SIZE, PROT_READ);
	pmap(1);

	return 0;
}
