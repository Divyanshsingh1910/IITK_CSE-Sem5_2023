#include<ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
  int pages = 4096;

  // vm_area will be created without physical pages.
  char * lazy_alloc = mmap(NULL, pages*50, PROT_READ|PROT_WRITE, 0);
  if((long)lazy_alloc < 0)
  {
    // Testcase failed.
    printf("1. Test case failed \n");
    return 1;
  }
  
  // All accesses should result in page fault.
  for(int i = 0; i<50; i++)
  {
    lazy_alloc[(pages * i)] = 'X';
  }

  // Number of MMAP_Page_Faults should be 50 & 
  // Number of vm_area should 1
  pmap(0);

  for(int i = 0; i<50; i++)
  {
    // Reading the value from physical page. It should be same as written
    if(lazy_alloc[(pages * i)] != 'X')
    {
      // Testcase Failed;
      printf("2. Test case failed \n");
      return 0;
    }
  }
  // Number of MMAP_Page_Faults should be 50 & 
  // Number of vm_area should 1
  pmap(0);

  munmap(lazy_alloc+pages*5, pages*10);
  pmap(1);
  lazy_alloc[pages*6] = 'A';
 return 0;
}

/*
###########     VM Area Details         ################
        VM_Area:[2]             MMAP_Page_Faults[50]

        [0x180201000    0x180206000] #PAGES[5]  R W _
        [0x180210000    0x180233000] #PAGES[35] R W _

        ###############################################

do_page_fault: (Sig_Exit) PF Error @ [RIP: 0x100000E2C] [accessed VA: 0x180207000] [error code: 0x6]
*/
