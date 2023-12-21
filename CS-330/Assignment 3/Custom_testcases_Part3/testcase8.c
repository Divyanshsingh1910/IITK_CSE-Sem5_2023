#include <ulib.h>

/*simple cfork testcase */

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	char *mm1 = mmap(NULL, 45, PROT_READ, 0);
	for(int i=0;i<10;i++)
	{
		long ret = cfork();
		if(ret == 0)
		{
			printf("child %d\n", getpid());
			sleep(200);
			exit(0);
		}
		else
		{
			int r = mprotect(mm1, 40, PROT_READ|PROT_WRITE);
			mm1[0] = 'a';
			pmap(0);
				long x = get_user_page_stats();
			printf("user pg %x\n", x);
			printf("pid %d\n", getpid());

		}
	}

	long cow_fault = get_cow_fault_stats();
	printf("cow fault: %x \n", cow_fault);
	if (cow_fault > 0)
		printf("Testcase passed\n");
	else
		printf("Testcase failed\n");
	return 0;
}

/*
child 2
VM_Area:[1]     MMAP_Page_Faults[1]
user pg 0xD
pid 1
child 3
VM_Area:[1]     MMAP_Page_Faults[2]
user pg 0x11
pid 1
child 4
VM_Area:[1]     MMAP_Page_Faults[3]
user pg 0x14
pid 1
child 5
VM_Area:[1]     MMAP_Page_Faults[4]
user pg 0x17
pid 1
child 6
VM_Area:[1]     MMAP_Page_Faults[5]
user pg 0x1A
pid 1
child 7
VM_Area:[1]     MMAP_Page_Faults[6]
user pg 0x1D
pid 1
child 8
VM_Area:[1]     MMAP_Page_Faults[7]
user pg 0x20
pid 1
child 9
VM_Area:[1]     MMAP_Page_Faults[8]
user pg 0x23
pid 1
child 10
VM_Area:[1]     MMAP_Page_Faults[9]
user pg 0x26
pid 1
child 11
VM_Area:[1]     MMAP_Page_Faults[10]
user pg 0x29
pid 1
cow fault: 0x2F
Testcase passed
*/
