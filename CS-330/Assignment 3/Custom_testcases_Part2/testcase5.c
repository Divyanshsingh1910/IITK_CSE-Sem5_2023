#include<ulib.h>
#define PAGE 4096

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	char *area = mmap(NULL, PAGE, PROT_READ|PROT_WRITE, 0);
	if(area == -1){
		printf("fail1\n");
		return -1;
	}
	pmap(1);
	for(int i = 0; i<PAGE; i++){
		*(area + i) = 'A';
	}
	pmap(1);
	int ret = mprotect(area, 1, PROT_READ);
	if(ret < 0){
		printf("fail2");
		return -1;
	}
	pmap(1);
	for(int i = 0; i<PAGE; i++){
		if(area[i] != 'A'){
			printf("fail 3");
			return -1;
		}
	}
	area[10] = 'B';
	printf("fail");
	return -1;
}

/*


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

        [0x180201000    0x180202000] #PAGES[1]  R _ _

        ###############################################

do_page_fault: (Sig_Exit) PF Error @ [RIP: 0x100000E1F] [accessed VA: 0x18020100A] [error code: 0x7]
GemOS# 
*/
