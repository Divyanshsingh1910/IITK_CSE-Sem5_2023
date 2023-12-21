#ifndef __FILE_H_
#define __FILE_H_

#include<context.h>

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
    PIPE,
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
    struct fileops * fops;
    struct pipe_info * pipe;
};

struct fileops{
    int (*read)(struct file *filep, char * buff, u32 count);
    int (*write)(struct file *filep, char * buff, u32 count); //seek implementation
    long (*lseek)(struct file *filep, long offset, int whence);
    long (*close)(struct file *filep);
};

//STDIO handlers and functions
extern struct file *alloc_file();
extern struct file* create_standard_IO(int);
extern int open_standard_IO(struct exec_context *ctx, int type);
extern void do_file_fork(struct exec_context *child);
extern void do_file_exit(struct exec_context *ctx);

//Reg file read and writ
extern int do_regular_file_open(struct exec_context *ctx, char *filename, u64 flags, u64 mode);

// Dup, Dup2
extern int fd_dup(struct exec_context *current, int oldfd);
extern int fd_dup2(struct exec_context *current, int oldfd, int newfd);
extern long std_close(struct file *filep);

//Generic handlers

extern int do_file_open(struct exec_context *ctx,u64 filename, u64 flag, u64 mode);
extern int do_file_read(struct exec_context *ctx, u64 fd, u64 buff, u64 count);
extern int do_file_write(struct exec_context *ctx, u64 fd, u64 buff, u64 count);
extern int do_create_pipe(struct exec_context *ctx, int*fd);
extern int do_dup(struct exec_context *ctx, int oldfd);
extern int do_dup2(struct exec_context *ctx, int oldfd, int newfd);
extern int do_close(struct exec_context *ctx, int fd);
extern long do_lseek(struct exec_context *ctx, int fd, long offset, int whence);

#endif
