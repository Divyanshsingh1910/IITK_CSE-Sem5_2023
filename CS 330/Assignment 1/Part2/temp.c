#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/dir.h>
#include <assert.h>
#include <string.h>


int main(int argc, char *argv[])
{
	struct stat *fptr = (struct stat*)malloc(sizeof(struct stat));
	const char* pathname = argv[1];
	int w = lstat(pathname, fptr);
	printf("The Func returns %d\n",w);
	printf("Size of %s directory is: %ld\n",pathname,fptr->st_size);
	printf("Let's explore the contents of the directory\n");

	DIR *dp = opendir(pathname);
	assert(dp != NULL);

	struct dirent *dptr;
	char* dir_name = argv[1];
	char slash[] = "/";
	strcat(dir_name,slash);

	while( (dptr = readdir(dp)) != NULL) {
		printf("`%lu` has `%s` which is of type: `%d`\n", (unsigned long) dptr->d_ino, dptr->d_name, dptr->d_type);
		
		char new_pathname[100];
		strcpy(new_pathname,dir_name);
		
		strcat(new_pathname,dptr->d_name);
		printf("%s\n",new_pathname);

		struct stat* newfp = (struct stat*)malloc(sizeof(struct stat));
		int k = lstat(new_pathname, newfp);
		printf("-----------> size of %s is %ld\n\n",dptr->d_name, newfp->st_size);

	}  
	return 0;
}