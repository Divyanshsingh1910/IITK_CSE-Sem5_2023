#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
int main()
{
   int pid;
   char buf[32];
   int fd[2];
   if(pipe(fd) < 0){
        perror("pipe");
        exit(-1);
   }

   pid = fork();   
   if(pid < 0){
      perror("fork");
      exit(-1);
   }  

   if(!pid){ // Child 
       close(fd[1]);      // Close the write end in child
       close(0);   // Close the standard input
       dup(fd[0]);  // Now 0 is fd[0]!
       execl("/usr/bin/wc", "wc", "-l", NULL);
       perror("execl");
       exit(-1);
   }
   
   close(fd[0]);    // Close the read end in the parent
   close(1);        // Close the standard output
   dup(fd[1]);      // Now 1 is fd[1]!
   execl("/bin/ls", "ls",  NULL);
   perror("execl");
   exit(-1);
}
/* 
 * The above code executes the command " ls | wc -l"
 * basically closing the write end of pipe in child and 
 * closing the read end in the parent making it a one directional channel
 * also then assigning the stdin and stdout fds to the read and write 
 * end of the pipe
 */
