#include<ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
  int pages = 4096;

  // vm_area will be created without physical pages.
  char * lazy_alloc = mmap(NULL, pages*50, PROT_READ|PROT_WRITE, 0);
  if((long)lazy_alloc < 0)
  {
    // Testcase failed.
    printf("Test case failed \n");
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
      printf("Test case failed \n");
      return 0;
    }
  }
  // Number of MMAP_Page_Faults should be 50 & 
  // Number of vm_area should 1
  pmap(0);

 return 0;
}
