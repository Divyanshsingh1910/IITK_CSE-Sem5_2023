#include<ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{

  int pages = 4096;

  char * mm1 = mmap(NULL, pages*6, PROT_READ|PROT_WRITE, 0);
  if((long)mm1 < 0)
  {
    // Testcase failed.
     printf("Test case failed \n");
    return 1;
  }
  // vm_area count should be 1.
  pmap(1);

  unsigned long readonly_address = (unsigned long)mm1 + pages* 3;

  // Should change access rights of the third page, existing vm_Area should be splitted up. A new vm area with access rights with PROT_READ will be created.
  int result  = mprotect((void *)readonly_address, pages, PROT_READ);

  if(result <0)
  {
    // Testcase failed
     printf("Test case failed \n");
    return 0;
  }
  
  // vm_area count should be 3.
  pmap(1);

  return 0;
}
