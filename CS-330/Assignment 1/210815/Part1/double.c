#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 1000

int main(int argc, char *argv[])
{
	char buf[BUF_SIZE];
	unsigned long n, doubled_n;
	strcpy(buf, argv[argc - 1]);
	n = strtoul(buf, NULL, 10);
	doubled_n = 2*n;
	if(argc == 2){
		printf("%lu\n", doubled_n);
		exit(0);
	}
	sprintf(buf, "%lu", doubled_n);
	char *new_argv[argc];
	for(int i = 1; i < argc; i++){
		new_argv[i - 1] = argv[i];
	}
	new_argv[argc - 2] = buf;
	new_argv[argc - 1] = NULL;
	execv(argv[1], new_argv);
	printf("Unable to execute\n");
	exit(-1);
}