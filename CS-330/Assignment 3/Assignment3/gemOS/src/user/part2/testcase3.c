#include<ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
  int pages = 4096;
  char * mm1 = mmap(NULL, pages*6, PROT_READ, 0);
  if((long)mm1 < 0)
  {
    // Testcase failed.
     printf("Test case failed \n");
    return 1;
  }
  //Read all the pages
  for(int i = 0; i < 6; i++){
        char temp;
        char* page_read = mm1 +(i*pages);
        temp = page_read[0];
  }
  //Page faults should be 6
  pmap(0);
  //changing the protection should give access to write
  int result  = mprotect((void *)mm1, pages*6, PROT_READ|PROT_WRITE);
  if(result <0)
  {
     printf("Test case failed \n");
    return 0;
  }

  for(int i = 0; i < 6; i++){
        char* page_write = mm1 +(i*pages);
        page_write[0] = 'A';
  }
  //Page faults should be 6
  pmap(0);

  return 0;
}

