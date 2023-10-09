#include<entry.h>
#include<lib.h>
#include<context.h>
#include<memory.h>


extern void cfork_copy_mm(struct exec_context *child, struct exec_context *parent );
extern void vfork_copy_mm(struct exec_context *child, struct exec_context *parent );
extern int handle_cow_fault(struct exec_context *current, u64 cr2);
extern void vfork_exit_handle(struct exec_context *ctx);
