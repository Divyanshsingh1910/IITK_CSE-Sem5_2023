#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
int main(void)
{
   char buf[32];
   int fd[2];
   if(pipe(fd) < 0){
        perror("pipe");
        exit(-1);
   }
   if(write(fd[1], "Hello pipe\n", 11) != 11){
       perror("write");
       exit(-1);
   }
   if(read(fd[0], buf, 11) != 11){
       perror("write");
       exit(-1);
   }
   
  buf[11] = '\0';
  printf("%s", buf);
}
