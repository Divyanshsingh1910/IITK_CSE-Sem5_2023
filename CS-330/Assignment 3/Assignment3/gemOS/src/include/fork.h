#ifndef __FORK_H_
#define __FORK_H_

#include<entry.h>
#include<lib.h>
#include<context.h>
#include<memory.h>


extern void setup_child_context(struct exec_context *child);
extern void copy_os_pts(u64 src, u64 dst);
extern void vfork_exit_handle(struct exec_context *ctx);

extern long do_fork(void);
extern long do_cfork(void);
extern long do_vfork(void);
extern long do_clone(void *th_func, void *user_stack, void *user_arg);

#endif
