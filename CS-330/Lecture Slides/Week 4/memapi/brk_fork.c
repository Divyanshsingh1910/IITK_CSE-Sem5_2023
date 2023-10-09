#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

extern char etext, edata, end; /*see man(3) end*/
int g_arr[1024];

int main()
{
   int pid;
   printf("Current heap start = %p\n", sbrk(0));
   if(sbrk(4096 * 1024) == (void *)-1){
        printf("sbrk failed\n");
   }
   printf("Heap start after expand = %p\n", sbrk(0)); 
   pid = fork();
   if(!pid){ //child
       printf("Child: Current heap start = %p\n", sbrk(0));
       if(sbrk(4096 * 1024) == (void *)-1)
             printf("sbrk failed\n");
       printf("Child: heap start after expand = %p\n", sbrk(0));
       exit(0);
   }
   wait(NULL);
   printf("Heap start after child = %p\n", sbrk(0)); 
}
