#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/syscall.h>

int main(void)
{
  long pid = 0;
  
  pid = getpid();
  printf("LIB_getpid: pid = %ld\n", pid);
  pid = 0;

  pid = syscall(SYS_getpid);
  printf("WRAP_getpid: pid = %ld\n", pid);
  pid = 0;
  
  asm ("mov $39, %%rax;"    /*SYSCALL GETPID = 39  defined in /usr/include/x86_64-linux-gnu/asm/unistd_64.h*/
               "syscall;"
	       "mov %%rax, %0;"
               :"=m" (pid)
	       :
	       :         
  );
  printf("INST_getpid: pid = %ld\n", pid);
}
