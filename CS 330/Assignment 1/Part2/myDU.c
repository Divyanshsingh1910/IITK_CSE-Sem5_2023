#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/dir.h>
#include <assert.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

int SIZE_DIR(char *DIR_path){
	DIR *dp = opendir(DIR_path);
	// printf("Opening %s\n",DIR_path);
	assert(dp != NULL);
	struct dirent *dptr;
	// sizeee
	struct stat *fptr = (struct stat*)malloc(sizeof(struct stat));
	const char* pathname = DIR_path;
	int w = lstat(pathname, fptr);
	int size = 0;
	size += fptr->st_size;

	// printf("Size of %s is %d\n",DIR_path,size);
	char* dir_name = DIR_path;
	strcat(dir_name,"/");
	while( (dptr = readdir(dp)) != NULL){

		char new_pathname[1000];

		int len = strlen(dir_name);

		for(int i=0; i<len; i++){
			new_pathname[i]=dir_name[i];
		}
		new_pathname[len]='\0';

		strcat(new_pathname,dptr->d_name);
		struct stat* newfp = (struct stat*)malloc(sizeof(struct stat));
		int k = lstat(new_pathname, newfp);

		if(strcmp(dptr->d_name,".") && strcmp(dptr->d_name,"..")){
			
			if(dptr->d_type==4){
				// pipe when a sub-folder encountered
				size+= SIZE_DIR(new_pathname);
				// printf("In FUNC size of %s is %d\n",new_pathname,SIZE_DIR(new_pathname));

			}
			if(dptr->d_type==8){
				// regular file
				size+= newfp->st_size;
				// printf("In FUNC size of %s is %ld\n",new_pathname,newfp->st_size);
			}
			else if(dptr->d_type==10){
				
				
				// symbolic link
				// size+= newfp->st_size;
				// continue;
				
				char resolvedPath[1024];  // Assuming a reasonable buffer size
				
				// Use readlink to get the target path
				// printf("Link file: %s\n",new_pathname);
				ssize_t len = readlink(new_pathname, resolvedPath, sizeof(resolvedPath) - 1);
				
				if (len != -1) {
					resolvedPath[len] = '\0';  // Null-terminate the string
					// //// // printf("The target of the symbolic link is: %s\n", resolvedPath);
				} else {
					perror("Error reading symbolic link");
					return 1;
				}
				// printf("Resolved path:= %s\n",resolvedPath);

				char relativePath[1024];
				len = strlen(dir_name);
				for(int i=0; i<len; i++){
					relativePath[i]=dir_name[i];
				}
				relativePath[len]='\0';
				strcat(relativePath,resolvedPath);
				// printf("Relative path for target link is %s\n",relativePath);

				struct stat* symfp = (struct stat*)malloc(sizeof(struct stat));
				int St = lstat(relativePath, symfp);
				if(St!=0){
					perror("Error in lstat call!");
				}
				if(S_ISDIR(symfp->st_mode)){
					// printf("It is a Directory\n");
					size += SIZE_DIR(relativePath);
					// printf("size of resolvedPath: %s is %d\n",resolvedPath,SIZE_DIR(relativePath));
				}
				else{
					// printf("%s is a regular file\n",resolvedPath);
					size+= newfp->st_size;
				}
				
			}
		}		
	}

	// ----------------
	// printf("Complete size of %s is %d\n",DIR_path,size);
	return size;
}

int main(int argc, char *argv[])
{
	struct stat *fptr = (struct stat*)malloc(sizeof(struct stat));
	const char* pathname = argv[1];
	int w = lstat(pathname, fptr);
	int size = 0;
	size += fptr->st_size;

	DIR *dp = opendir(pathname);
	assert(dp != NULL);

	struct dirent *dptr;
	char* dir_name = argv[1];
	strcat(dir_name,"/");

	while( (dptr = readdir(dp)) != NULL){

		char new_pathname[100];

		int len = strlen(dir_name);

		for(int i=0; i<len; i++){
			new_pathname[i]=dir_name[i];
		}
		new_pathname[len]='\0';

		strcat(new_pathname,dptr->d_name);
		struct stat* newfp = (struct stat*)malloc(sizeof(struct stat));
		int k = lstat(new_pathname, newfp);

		if(strcmp(dptr->d_name,".") && strcmp(dptr->d_name,"..")){
			
			if(dptr->d_type==4){
				// pipe when a sub-folder encountered
				int fd[2];
				int null = pipe(fd);	
				// new_pathname is directory hai
				int pid = fork();
				assert(pid>=0);
				
				if(!pid){
					int len = strlen(new_pathname);
					new_pathname[len]='\0';

					char* args[3];
					args[0] = "myDU.out";
					args[1] = new_pathname;
					args[2] = NULL;
					
					close(1); //stdout closed for child;
					dup(fd[1]); //write end of this child changed to stdout

					execv(args[0],args);

				}
				else{
					// parent
					int w = wait(NULL);
					char buff[256];
					if(read(fd[0],buff,256)<=0){
						//// // printf("%s parent couldn't read %s child\n",pathname,new_pathname);
					}
					// printf("size of %s is %d\n",new_pathname,atoi(buff));
					size += atoi(buff);
				}
			}
			if(dptr->d_type==8){
				// regular file
				size+= newfp->st_size;
				// printf("size of %s is %ld\n",new_pathname,newfp->st_size);
			}
			else if(dptr->d_type==10){
				
				// symbolic link
				// size+= newfp->st_size;
				// continue;
				
				char resolvedPath[1024];  // Assuming a reasonable buffer size
				
				// Use readlink to get the target path
				// printf("Link file: %s\n",new_pathname);
				ssize_t len = readlink(new_pathname, resolvedPath, sizeof(resolvedPath) - 1);
				
				if (len != -1) {
					resolvedPath[len] = '\0';  // Null-terminate the string
					// //// // printf("The target of the symbolic link is: %s\n", resolvedPath);
				} else {
					perror("Error reading symbolic link");
					return 1;
				}
				// printf("Resolved path:= %s\n",resolvedPath);

				char relativePath[1024];
				len = strlen(dir_name);
				for(int i=0; i<len; i++){
					relativePath[i]=dir_name[i];
				}
				relativePath[len]='\0';
				strcat(relativePath,resolvedPath);
				// printf("Relative path for target link is %s\n",relativePath);

				struct stat* symfp = (struct stat*)malloc(sizeof(struct stat));
				int St = lstat(relativePath, symfp);
				if(St!=0){
					perror("Error in lstat call!");
				}
				if(S_ISDIR(symfp->st_mode)){
					// printf("It is a Directory\n");
					size += SIZE_DIR(relativePath);
					// printf("size of resolvedPath: %s is %d\n",resolvedPath,SIZE_DIR(relativePath));
				}
				else{
					// printf("%s is a regular file\n",resolvedPath);
					size+= newfp->st_size;
				}
				
			}
		}		
	}
	char result[256];
	sprintf(result,"%d",size);
	write(1,result,strlen(result));
	return 0;
}
