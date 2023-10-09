#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
int main(void)
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
       if(write(fd[1], "Hello pipe\n", 11) != 11){
            perror("write");
            exit(-1);
        }
       exit(0);
   }
   
   if(read(fd[0], buf, 11) != 11){
       perror("write");
       exit(-1);
   }
   
  buf[11] = '\0';
  printf("In parent: %s", buf);
  if(write(fd[1], "Hello0pipe\n", 50) ==0){
            perror("write");
            exit(-1);
  }
 if(read(fd[0], buf, 32) == 0){
       perror("write");
       exit(-1);
   }

  buf[11] = '\0';
  printf("Now in In parent: %s", buf);

}
/*
 * The code shows how we can initialize the pipe with two
 * file descriptors and writing from one and reading from
 * other fd works
 */
