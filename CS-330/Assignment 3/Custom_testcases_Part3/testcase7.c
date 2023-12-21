#include<ulib.h>
#define PAGE 4096

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	int x = 0;
	printf("in main: %d\n", x);
	int *arr = mmap(NULL, PAGE*4, PROT_READ|PROT_WRITE, 0);
	pmap(0);
	for(int i = 0; i<PAGE; i++){
		arr[i] = i;
	}
	pmap(0);
	
	int retfork = cfork();

	if(retfork == 0){
		// child
		int cowcnt = get_cow_fault_stats();
		printf("cow count = %d, pid = %d\n", cowcnt, getpid());
		x = 100;
		for(int i = 0; i<PAGE; i++){
			arr[i] = -1;
		}
		cowcnt = get_cow_fault_stats();
		printf("cow count = %d, pid = %d\n", cowcnt, getpid());
		sleep(2000);
		for(int i = 0; i<PAGE; i++){
			arr[i] = 0;
		}
		printf("arr: %d pid %d\n", arr[1], getpid());
	}
	else{
		sleep(1000);
		printf("x: %d\n", x);
		printf("arr: %d\n", arr[1]);
		munmap(arr, 4*PAGE);
		exit(0);
	}
	
	return 0;
}


/*
 *
in main: 0
VM_Area:[1]     MMAP_Page_Faults[0]
VM_Area:[1]     MMAP_Page_Faults[4]
cow count = 1, pid = 2
cow count = 7, pid = 2
x: 0
arr: 1
arr: 0 pid 2
GemOS# 
 */
