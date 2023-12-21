#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
int main(void){
   void *ptr = NULL, *p_start = sbrk(0);
   int a = 100;
   pid_t pid = fork(); //Assume success
   if(!pid){ //child
       ptr = sbrk(4096);
       printf("%p\n", &a);
   }else{
       if(ptr){
            printf("yes\n");
       }
       ptr = sbrk(8192); 
       printf("%p\n", &a);
   }
   printf("%ld\n", ptr - p_start);
}
/*  (1 + 1 + 3 + 5 = 10) 
Q1. Will the values printed in line #10 and line #16 be same? (Yes/No)
Q2. The `printf' at line#13 will never be executed? (True/False) 
Q3. What will be the value(s) printed in line#18? (Assume no sbrk calls from C library after start of main()) 
Q4. Assume that, the child's program counter is modified to jump to the instruction at line #12 after successful execution of sbrk() at line #9. What will be the program behavior and why? (provide brief reasoning) 
*/
