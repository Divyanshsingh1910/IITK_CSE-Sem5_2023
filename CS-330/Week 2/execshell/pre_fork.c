#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(void)
{
    pid_t pid;
    printf("Before calling fork\n");
    pid = fork();
    if(pid == 0){
	 printf("pid<0 must be a chilld\n");
         perror("fork");
         exit(-1);
    }
   printf("After calling fork pid = %d\n", pid);
}
