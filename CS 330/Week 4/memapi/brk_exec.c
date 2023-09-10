#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

extern char etext, edata, end; /*see man(3) end*/
int g_arr[1024];

int main()
{
   int pid;
   printf("End of text %p\n", &etext);
   printf("End of initialized data %p\n", &edata);
   printf("End of uninitialized data %p\n", &end);
   printf("Current heap start = %p\n", sbrk(0));
   if(sbrk(4096 * 1024) == (void *)-1){
        printf("sbrk failed\n");
   }
   printf("Heap start after expand = %p\n", sbrk(0)); 
   pid = fork();
   if(!pid){ //child
       execl("./brk", "brk", NULL);
       perror("execl");
   }else{
      wait(NULL);
   }
}
