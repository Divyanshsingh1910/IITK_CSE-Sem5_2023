#ifndef __FILE_H_
#define __FILE_H_
#include<context.h>
#include<tracer.h>
#define   O_READ 0x1
#define   O_WRITE 0x2
#define   O_RDWR (O_READ|O_WRITE)
#define   O_EXEC  0x4
#define   O_CREAT 0x8



#define CREATE_READ O_READ
#define CREATE_WRITE O_WRITE
#define CREATE_EXEC O_EXEC


enum{
	STDIN,
	STDOUT,
	STDERR,
	REGULAR,
	TRACE_BUFFER,
	MAX_FILE_TYPE,
};

enum{
	SEEK_SET,
	SEEK_CUR,
	SEEK_END,
	MAX_SEEK,
};

struct file{
	u32 type;
	u32 mode;
	u32 offp;
	u32 ref_count;
	struct inode * inode;
        struct trace_buffer_info *trace_buffer;// Incase of Regular file, It will be null
	struct fileops * fops;
};


struct fileops{
	int (*read)(struct file *filep, char * buff, u32 count);
	int (*write)(struct file *filep, char * buff, u32 count); //seek implementation
	long (*lseek)(struct file *filep, long offset, int whence);
	long (*close)(struct file *filep);
};

//STDIO handlers and functions
extern struct file *alloc_file();
extern void *alloc_memory_buffer();
extern struct file* create_standard_IO(int);
extern int open_standard_IO(struct exec_context *ctx, int type);
extern int do_file_fork(struct exec_context *child);
extern void do_file_exit(struct exec_context *ctx);

//Reg file read and writ
extern int do_regular_file_open(struct exec_context *ctx, char *filename, u64 flags, u64 mode);

// Dup, Dup2
extern int fd_dup(struct exec_context *current, int oldfd);
extern int fd_dup2(struct exec_context *current, int oldfd, int newfd);
extern long std_close(struct file *filep);
extern long file_close(struct file *filep);

#endif
