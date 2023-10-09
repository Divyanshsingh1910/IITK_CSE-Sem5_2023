#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>

#define BUF_SIZE 1000
#define PATH_SIZE 1000

void create_path(char path[], char parent_path[], char name[]){
    int n = strlen(parent_path), m = strlen(name);
    for(int i = 0; i < n; i++){
        path[i] = parent_path[i];
    }
    path[n] = '/';
	for(int i = 0; i < m; i++){
        path[n + 1 + i] = name[i];
    }
	path[n + m + 1] = NULL;
}

unsigned long find_file_size(char file_path[]){
    int fd = open(file_path, O_RDONLY);
    if(fd < 0){
        return -1;
    }
    unsigned long size = lseek(fd, 0, SEEK_END);
    return size;
}

unsigned long find_directory_size(int argc, char *argv[]){
	char buf[BUF_SIZE];
	char path[PATH_SIZE];
	DIR *dp = opendir(argv[1]);
	if(dp == NULL){
        return -1;
	}
	unsigned long size = 0;
	struct dirent *d;
	while((d = readdir(dp)) != NULL){
		create_path(path, argv[1], d->d_name);
		if(d->d_type == DT_REG){ // file
			unsigned long curr_size = find_file_size(path);
            if(curr_size == -1){
                return -1;
            }
            size += curr_size;
		}
		else if(d->d_type == DT_LNK){ // symbolic link
			int len = readlink(path, buf, BUF_SIZE);
			buf[len] = NULL;
			create_path(path, argv[1], buf);
			struct stat sb;
			int status = stat(path, &sb);
			if(status < 0){
				return -1;
			}
			if(S_ISREG(sb.st_mode)){
				unsigned long curr_size = find_file_size(path);
				if(curr_size == -1){
					return -1;
				}
				size += curr_size;
			}
			else{
				char *new_argv[3];
				new_argv[0] = argv[0];
				new_argv[1] = path;
				new_argv[2] = NULL;
				unsigned long curr_size = find_directory_size(argc, new_argv);
				if(curr_size < 0){
					return -1;
				}
				size += curr_size;
			}
		}
		else{// directory
			if(strcmp(d->d_name, ".") == 0){
				struct stat sb;
				int status = stat(path, &sb);
				if(status < 0){
                    return -1;
				}
				size += sb.st_size;
				continue;
			}
			else if(strcmp(d->d_name, "..") == 0){
				continue;
			}
			else{
				if(argc == 2){
					int fd[2];
					pipe(fd);
					int pid = fork();
					if(pid < 0){
						return -1;
					}
					else if(pid == 0){
						close(fd[0]);
						char *new_argv[4];
						new_argv[0] = argv[0];
						new_argv[1] = path;
						sprintf(buf, "%d", fd[1]);
						new_argv[2] = buf;
						new_argv[3] = NULL;
						execv(argv[0], new_argv);
						return -1;
					}
					else{
						close(fd[1]);
						read(fd[0], buf, BUF_SIZE);
						unsigned long curr_size = strtoul(buf, NULL, 10);
						size += curr_size;
					}
				}
				else{
					char *new_argv[3];
					new_argv[0] = argv[0];
					new_argv[1] = path;
					new_argv[2] = NULL;
					unsigned long curr_size = find_directory_size(argc, new_argv);
					if(curr_size < 0){
						return -1;
					}
					size += curr_size;
				}
			}
		}
	}
	return size;
}

int main(int argc, char *argv[]){
	unsigned long size = find_directory_size(argc, argv);
	if(size == -1){
		printf("Unable to execute\n");
        exit(-1);
	}
	if(argc == 2){
		printf("%lu\n", size);
	}
	else{
		char buf[BUF_SIZE];
		sprintf(buf, "%lu", size);
		write(atoi(argv[2]), buf, BUF_SIZE);
	}
	exit(0);
}