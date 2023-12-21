#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
int main(void)
{
  int i;
  int fd = open("quiz1.c", O_RDONLY); 
  for(i=0; i<2; ++i){
        int dfd;
	fork();  
        dfd = dup(fd);
  }
  exit(0);
}
/* Consider that process P starts execution of the main() function. Assuming all system calls are executed successfully, answer the following questions. (2 + 3 + 4 + 1 = 10)

 * 1. What is the maximum and minimum number of actively used PCBs during the execution of the above program? 

 * 2. Assuming only the default FDs (0, 1 and 2) are used by P at the start of its execution, what will be the content of the FD table of P just before P exits?    

 * 3. Let us define R to be the number of FDs that refer to the file object corresponding to the file "quiz1.c". What is the maximum value of R during the execution of the above program?

 * 4. Will the change in R be same for exit of each process?
 */
