#include<ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{

  int pages = 4096;

  char * mm1 = mmap(NULL, pages*2, PROT_READ|PROT_WRITE, 0);
  if((long) mm1 < 0)
  {
    printf("Test case failed \n");
    return 1;
  }
  // vm_area count should be 1
  pmap(0);

  // The below mmap request will be merged with the existing vm_area as the access flags are same as previous vm_area
  // mm2 address should be the next immediate address of (mm1 + pages*2);
  char * mm2 = mmap(NULL, pages*2, PROT_READ|PROT_WRITE, 0);

 
  char * check_addr = mm1 + (pages*2);
  
  if((long)mm2 < 0 &&  (long)mm2 != (long)check_addr)
  {
    printf("Test case failed \n");
    return 1;
  }

  // vm_area count should be 1, It will be merged with existing area
  pmap(0);

  char * mm3 = mmap(NULL, pages*4, PROT_READ, 0);
  if((long)mm3 < 0)
  {
    printf("Test case failed \n");
    return 1;
  }

  // New vm_area should be created as access rights are different.
  // vn_area count should be 2
  pmap(0);

  int val1 = munmap(mm3, pages*2);
  if(val1 < 0)
  {
    printf("Test case failed \n");
    return 1;
  }

  // vm_area count should be 2. 
  pmap(0);

  char * mm4 = mmap(NULL, pages*2, PROT_READ, 0);
    if((long)mm4 < 0)
  {
    printf("Test case failed \n");
    return 1;
  }

  // vm_area count should be 2
  pmap(0);

  int val = munmap(mm4 + pages , pages*2);
  if(val < 0)
  {
    printf("Test case failed \n");
    return 1;
  }

  // Area unmapped is inside the vm_area. So vm_area will be splitted into two.
  // vm_area count should be 3
  pmap(0);

  return 0;
}
