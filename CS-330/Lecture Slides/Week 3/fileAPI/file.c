#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<fcntl.h>
#include<assert.h>

int main(void)
{
   int fd1, fd2, pid;
   char buf[32];
   
   fd1 = open("file.c", O_RDONLY);
   if(fd1 < 0){
       perror("open");
       exit(-1);
   }

   fd2 = open("file.c", O_RDONLY);
   if(fd2 < 0){
       perror("open");
       exit(-1);
   }
   
  assert(read(fd1, buf, 32) == 32);
  
  printf("fd1 position = %ld, fd2 position = %ld\n", lseek(fd1, 0, SEEK_CUR), lseek(fd2, 0, SEEK_CUR));  
  assert(read(fd2, buf, 32) == 32);
  printf("fd1 position = %ld, fd2 position = %ld\n", lseek(fd1, 0, SEEK_CUR), lseek(fd2, 0, SEEK_CUR));  
}
