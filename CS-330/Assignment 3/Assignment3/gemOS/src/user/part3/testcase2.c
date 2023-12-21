#include<ulib.h>

/*mmap and call cfork testcase. Parent should be able to print mm1[0]
even after child called munmap */

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
  long pid;
  int pages = 4096;
  char * mm1 = mmap(NULL, pages*2, PROT_READ|PROT_WRITE, 0);
  if(mm1 < 0)
  {
    printf("Testcase failed \n");
    return 1;
  }
  mm1[0] = 'A';
  pid = cfork();
  if(pid){
      sleep(60);
      if(mm1[0] != 'A')
      {
        printf("Testcase failed \n");
        return 1;
      }
      printf("Parent mm1[0]:%c\n",mm1[0]);
  }
  else{
      mm1[0] = 'B';
      printf("Child mm1[0]:%c\n",mm1[0]);
      int val1 =  munmap(mm1, pages*2);
      if(val1 < 0)
      {
        printf("Testcase failed\n");
        return 1;
      }
      exit(0);
  }

  return 0;
}

