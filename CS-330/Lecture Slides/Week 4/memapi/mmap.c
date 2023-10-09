#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/mman.h>
#include <unistd.h>

int main()
{
   char *ptr;

   ptr = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, 0, 0);
   if(ptr == MAP_FAILED){
        perror("mmap");
        exit(-1);
   }
  printf("---For Char---\n");
  printf("ptr = %p\n", ptr);
  printf("ptr + 1 = %p\n",ptr+1);

  unsigned long int *p;
  p = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, 0, 0);
   if(p == MAP_FAILED){
        perror("mmap");
        exit(-1);
   }
  printf("\n---For Unsigned Long Int---\n");
  printf("ptr = %p\n", p);
   printf("ptr + 1 = %p\n", p+1);


  
  strcpy(ptr, "hello cs330!");
  munmap((void *)ptr, 4096);
  return 0;
}
