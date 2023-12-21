#ifndef __ENTRY_H_
#define __ENTRY_H_
#include <types.h>
#include <context.h>

#define SYSCALL_EXIT      1
#define SYSCALL_GETPID    2
#define SYSCALL_EXPAND    4
#define SYSCALL_SHRINK    5
#define SYSCALL_ALARM     6
#define SYSCALL_SLEEP     7
#define SYSCALL_SIGNAL    8
#define SYSCALL_CLONE     9
#define SYSCALL_FORK      10
#define SYSCALL_STATS     11
#define SYSCALL_CONFIGURE 12
#define SYSCALL_PHYS_INFO   13
#define SYSCALL_DUMP_PTT    14
#define SYSCALL_CFORK       15
#define SYSCALL_MMAP        16
#define SYSCALL_MUNMAP      17
#define SYSCALL_MPROTECT    18
#define SYSCALL_PMAP        19
#define SYSCALL_VFORK      20
#define SYSCALL_GET_USER_P 21
#define SYSCALL_GET_COW_F 22
#define SYSCALL_OPEN        23
#define SYSCALL_READ        24
#define SYSCALL_WRITE       25
#define SYSCALL_PIPE        26
#define SYSCALL_DUP         27
#define SYSCALL_DUP2        28
#define SYSCALL_CLOSE       29
#define SYSCALL_LSEEK       30


#define SYSCALL_MAKE_THREAD_READY 31
#define SYSCALL_WAIT_FOR_THREAD 32


extern long do_syscall(long syscall, u64 param1, u64 param2, u64 param3, u64 param4, struct user_regs *regs);
extern int handle_div_by_zero(struct user_regs *regs);
extern int handle_page_fault(struct user_regs *regs);
extern void do_exit(u8);
#endif //__ENTRY_H
