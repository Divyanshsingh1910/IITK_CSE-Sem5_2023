#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(void)
{
    int i = 0;
    for(i=0; i<3; ++i){
          printf("%s: mypid = %d\n", __FILE__, getpid());
	  sleep(1);
    }	  
    exit(0);
}
