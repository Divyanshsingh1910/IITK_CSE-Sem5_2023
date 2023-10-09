#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(void)
{
    int i=5;
    pid_t pid;
    printf("Before calling fork mypid = %d\n", getpid());
    pid = fork();
    if(pid < 0){
         perror("fork");
         exit(-1);
    }
   printf("After calling fork mypid=%d pid=%d\n", getpid(), pid);
   if(pid){  /*Parent*/
        printf("Parent i=%d\n", i);
	i=10;
	sleep(1);
   }else{
        printf("Child i=%d\n", i);
   }
}

/*As the memory is copied while fork
 * changes in variable in any branch 
 * won't affect the value of that 
 * variable in another branch*/
