#include<ulib.h>

/*simple cfork testcase */

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
  long pid;
  long *va = (long *)expand(100,MM_WR);
  *va = 10;
  printf("Main number:%d\n", *va);
  long cow_fault;

  pid = cfork();
  long pid2 = cfork();
  if(pid){
    if(!pid2){
    sleep(30);	
    long pid = getpid();
    *va = 100;
    printf("Parent child pid:%u\n",pid);
    printf("Parent child number:%d\n", *va);

    }
    else{
    sleep(60);	
    long pid = getpid();
    printf("Parent parent pid:%u\n",pid);
    printf("Parent parent number:%d\n", *va);
    }
  }
  else{
    if(pid2){
          sleep(15);	

      long pid = getpid();
      printf("Child parent pid:%u\n",pid);
      *va = 100;
      printf("Child parent number:%d\n", *va);
      }
    else{
       long pid = getpid();
      printf("Child child pid:%u\n",pid);
      *va = 100;
      printf("Child child number:%d\n", *va);
    }

  }
  
  sleep(100);
  cow_fault = get_cow_fault_stats();
  printf("cow fault: %x pid %x\n", cow_fault, getpid());
  if(cow_fault > 0)
      printf("Testcase passed\n");
  else
      printf("Testcase failed\n");
  return 0;
}