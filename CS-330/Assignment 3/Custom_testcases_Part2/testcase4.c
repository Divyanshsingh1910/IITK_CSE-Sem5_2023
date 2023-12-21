#include<ulib.h>
#define PAGE 4096

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	char *area = mmap(NULL, PAGE, PROT_READ|PROT_WRITE, 0);
	pmap(1);
	*area = 10;
	pmap(1);
	area[2] = 'A';
	pmap(1);
	printf("%c\n", area[2]);
	return 0;
}

/*
 *
        ###########     VM Area Details         ################
        VM_Area:[1]             MMAP_Page_Faults[0]

        [0x180201000    0x180202000] #PAGES[1]  R W _

        ###############################################



        ###########     VM Area Details         ################
        VM_Area:[1]             MMAP_Page_Faults[1]

        [0x180201000    0x180202000] #PAGES[1]  R W _

        ###############################################



        ###########     VM Area Details         ################
        VM_Area:[1]             MMAP_Page_Faults[1]

        [0x180201000    0x180202000] #PAGES[1]  R W _

        ###############################################

A
GemOS# 
 *
 * /
