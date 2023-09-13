#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char etext, edata, end; /*see man(3) end*/

int main()
{
   void *ptr = sbrk(0);
   printf("%s: End of text %p\n", __FILE__, &etext);
   printf("%s: End of initialized data %p\n", __FILE__, &edata);
   printf("%s: End of uninitialized data (at load time) %p\n", __FILE__, &end);
   printf("%s: End of uninitialized data now = %p at main %p\n", __FILE__, sbrk(0), ptr);
   
   if(sbrk(4096 * 16) == (void *)-1){
        printf("sbrk failed\n");
   }
   printf("%s: End of uninitialized data after expand = %p\n", __FILE__, sbrk(0)); 
}
