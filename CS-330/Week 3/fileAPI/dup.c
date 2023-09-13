#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<fcntl.h>
#include<assert.h>

int main(void)
{
  int fd, dupfd;
  char buf[32];
 
  fd = open("file.c", O_RDONLY);
  if(fd < 0){
       perror("open");
       exit(-1);
  }
 printf("fd = %d\n", fd);
 dupfd = dup(fd);
 printf("dupfd = %d\n", dupfd);
 assert(read(fd, buf, 32) == 32);
 
 printf("fd position = %ld, dupfd position = %ld\n", lseek(fd, 0, SEEK_CUR), lseek(dupfd, 0, SEEK_CUR));  
}
