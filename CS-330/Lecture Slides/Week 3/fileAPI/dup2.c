#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<fcntl.h>
#include<assert.h>

int main()
{
  int fd, dupfd;
  char buf[32];
 
  fd = open("cs330.log", O_RDWR|O_CREAT, 0644);
  if(fd < 0){
       perror("open");
       exit(-1);
  }
 dupfd = dup2(fd, 1);
 printf("dupfd = %d\n", dupfd);  
 close(fd);
}
