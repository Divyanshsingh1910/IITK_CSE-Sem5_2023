#ifndef __PIPE_H_
#define __PIPE_H_

#include "types.h"
#include "context.h"
#include "file.h"


#define PIPE_MAX_SIZE 4096

/** Pipe information structure */
struct pipe_info{
    int read_pos;       //Last Read position
    int write_pos;      // Last Write position
    char* pipe_buff;    // pipe buffer
    int buffer_offset;  // current buffer length
    int is_ropen;
    int is_wopen;
};


/*int pipe_read(struct file *filep, char * buff, u32 count);
int pipe_write(struct file *filep, char * buff, u32 count);
int pipe_close(struct file *filep);*/
extern int create_pipe(struct exec_context *current, int *fd);

#endif
