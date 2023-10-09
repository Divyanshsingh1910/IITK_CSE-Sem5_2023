#ifndef __TRACER_H_
#define __TRACER_H_

#include<context.h>
#include<file.h>
#include<types.h>


///////////////////////////////////////////////////////////////////////
///////////////////// Trace buffer functionality ///////////////////// 
/////////////////////////////////////////////////////////////////////
#define TRACE_BUFFER_MAX_SIZE 4096

//Trace buffer information structure
struct trace_buffer_info
{

};


extern int sys_create_trace_buffer(struct exec_context *current, int mode);
extern void free_trace_buffer_info(struct trace_buffer_info *p_info);


///////////////////////////////////////////////////////////////////////
//////////////////////// strace functionality ///////////////////////// 
///////////////////////////////////////////////////////////////////////

#define STRACE_MAX 16
#define FULL_TRACING 0
#define FILTERED_TRACING 1

enum{
	ADD_STRACE,
	REMOVE_STRACE,
	MAX_STRACE
};

struct strace_info{
	int syscall_num;
	struct strace_info *next;
};

struct strace_head{
	int count;
	int is_traced;  
        int strace_fd;
        int tracing_mode;
	struct strace_info *next;
	struct strace_info *last;	
};

struct file;
struct exec_context;

extern int sys_start_strace(struct exec_context *current, int fd, int tracing_mode);
extern int sys_end_strace(struct exec_context *current);
extern int sys_read_strace(struct file *filep, char *buff, u64 count);
extern int sys_strace(struct exec_context *current, int syscall_num, int action);
extern int perform_tracing(u64 syscall, u64 param1, u64 param2, u64 param3, u64 param4);



///////////////////////////////////////////////////////////////////////
//////////////////////// ftrace functionality ///////////////////////// 
///////////////////////////////////////////////////////////////////////

#define FTRACE_MAX 16
#define MAX_ARGS 5
#define PUSH_RBP_OPCODE 0x55
#define INV_OPCODE 0xFF 
#define END_ADDR 0x10000003B

//Commands
enum{
               ADD_FTRACE,
               REMOVE_FTRACE,
	       ENABLE_FTRACE,
	       DISABLE_FTRACE,
	       ENABLE_BACKTRACE,
	       DISABLE_BACKTRACE,
	       MAX_FTRACE
};
  
struct ftrace_info{
                 unsigned long faddr;
		 u8 code_backup[4];
		 u32 num_args;
		 int fd;
		 int capture_backtrace;
                 struct ftrace_info *next;               		 
};

struct ftrace_head{
	         long count;  
                 struct ftrace_info *next;               		 
                 struct ftrace_info *last;   		 
};

struct user_regs;

extern long do_ftrace(struct exec_context *current, unsigned long faddr, long action, long nargs, int fd_trace_buffer);
extern long handle_ftrace_fault(struct user_regs *regs);
extern int sys_read_ftrace(struct file *filep, char *buff, u64 count);


#endif







