#include<stdio.h>
#include<stdlib.h>
int main(void)
{
   int x;
   printf("Enter: ");
   scanf("%d", &x);
   fprintf(stderr, "x=%d\n", x);
   exit(0);
}
