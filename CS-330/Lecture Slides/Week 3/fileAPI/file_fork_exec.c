#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<fcntl.h>
#include<assert.h>
#include<wait.h>

int main(void)
{
   int fd, pid;
   char buf[32];
   
   fd = open("file.c", O_RDONLY);
// fd = open("file.c", O_RDONLY|O_CLOEXEC);
   if(fd < 0){
       perror("open");
       exit(-1);
   }

  assert(read(fd, buf, 32) == 32);
  
  printf("parent: fd position = %ld\n", lseek(fd, 0, SEEK_CUR));  
  pid = fork();
  if(!pid){
       if(execl("./2.out", "2.out", NULL))
                 perror("exec");
       exit(-1);
  }
 wait(NULL);
 printf("parent: fd position = %ld\n", lseek(fd, 0, SEEK_CUR));  
}
/*
 * This code demonstrates that the offset is respected in the exec call as well
 * while when opened with tha flag O_CLOEXEC the fd is not communicated accross
 * the exec call
 */
