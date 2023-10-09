#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<assert.h>

int main(void)
{
    int fd = 3;
    char buf[32];
    printf("execchild: fd position = %ld\n", lseek(fd, 0, SEEK_CUR));  
    assert(read(fd, buf, 32) == 32);
    printf("execchild: fd position = %ld\n", lseek(fd, 0, SEEK_CUR));  
    exit(0);
}
