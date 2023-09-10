#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char etext, edata, end; /*see man(3) end*/

int global_x = 5;
char gchar[32];

int main()
{
   int x = 5;
   int *ptr = &x;
   void *p;

   printf("==========Text=========\n");    
   printf("Address of main = %p\n", &main);
   printf("End of text %p\n", &etext);
   
   printf("==========Data(initialized)=========\n");    
   printf("Address of global_x = %p\n", &global_x);
   printf("End of initialized data %p\n", &edata);

   printf("==========Data(uninitialized)=========\n");    
   printf("Address of gchar= %p\n", gchar);
   printf("End of uninitialized data %p\n", &end);
   
   printf("==========Stack=========\n");    
   printf("Address of x = %p &p = %p\n", &x, &p);

   p = malloc(1024);
   printf("Address of p = %p\n", p);

   ptr -= 4096;
   *ptr = 50;
   
   printf("ptr at = %p value %d\n", ptr, *ptr);
}


