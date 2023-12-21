#include<ulib.h>

/*simple cfork testcase */

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
  long pid;
  long pid2;
  long *va = (long *)expand(100,MM_WR);
  *va = 10;
  printf("Main number:%d\n", *va);
  long cow_fault;
  for(int i=0;i<4;i++){
  pid = cfork();
  *va = *va+1;
  printf("pid return: %d pid: %d va: %d\n", pid, getpid(), *va);  
  }

  sleep(1000);
  cow_fault = get_cow_fault_stats();
  printf("cow fault: %x \n", cow_fault);
  if(cow_fault > 0)
      printf("Testcase passed\n");
  else
      printf("Testcase failed\n");
  return 0;
}