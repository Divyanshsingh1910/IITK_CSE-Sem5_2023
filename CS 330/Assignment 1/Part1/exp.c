#include <stdio.h>
#include<stdlib.h>
#include<math.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>

int main(int argc, char *argv[])
{   
    char *str1  = "Hello ";
    char *str2;
    str2 = str1;
    printf("%s\n",str2);
    return 0;
}