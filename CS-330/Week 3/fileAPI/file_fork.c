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
   if(fd < 0){
       perror("open");
       exit(-1);
   }

  assert(read(fd, buf, 32) == 32);
  
  printf("parent: fd position = %ld\n", lseek(fd, 0, SEEK_CUR));  
  pid = fork();
  if(!pid){
       printf("child: fd position = %ld\n", lseek(fd, 0, SEEK_CUR));  
       assert(read(fd, buf, 32) == 32);
       printf("child: fd position = %ld\n", lseek(fd, 0, SEEK_CUR));  
       exit(0);
  }
 wait(NULL);
 printf("parent: fd position = %ld\n", lseek(fd, 0, SEEK_CUR));  
}
/*
 * Most probably the above code demonstrate that the 
 * file object is shared accross fork as the fd position is changed
 * for parent and child as well
 *
 * sample output: 
 * ---------------
 * ---------------
 * parent: fd position = 32
 * child: fd position = 32
 * child: fd position = 64
 * parent: fd position = 64
 * ---------------
 */
