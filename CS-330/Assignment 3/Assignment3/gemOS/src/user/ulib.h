#ifndef __ULIB_H__
#define __ULIB_H__

#define NULL (void *)0
#define MAP_ERR (void *)-1

typedef unsigned int u32;
typedef 	 int s32;
typedef unsigned short u16;
typedef int      short s16;
typedef unsigned char u8;
typedef char	      s8;

typedef __builtin_va_list va_list;
#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)

#if __x84_64__
typedef unsigned long u64;
typedef long s64
#else
typedef unsigned long long u64;
typedef long long s64;
#endif

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
#define SYSCALL_PHYS_INFO 13
#define SYSCALL_DUMP_PTT  14
#define SYSCALL_CFORK     15
#define SYSCALL_MMAP      16
#define SYSCALL_MUNMAP    17
#define SYSCALL_MPROTECT  18
#define SYSCALL_PMAP      19
#define SYSCALL_VFORK     20
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


#define MM_RD 0x1
#define MM_WR 0x2
#define MM_EX 0x4


#define NONE 0
#define MAP_FIXED 1
#define MAP_POPULATE 2

#define PROT_READ MM_RD
#define PROT_WRITE  MM_WR
#define PROT_EXEC MM_EX


#define   O_READ 0x1
#define   O_RDONLY O_READ
#define   O_WRITE 0x2
#define   O_WRONLY O_WRITE
#define   O_RDWR (O_READ|O_WRITE)
#define   O_EXEC  0x4
#define   O_CREAT 0x8

#define CREATE_READ O_READ
#define CREATE_WRITE O_WRITE
#define CREATE_EXEC O_EXEC


enum{
      SEEK_SET,
      SEEK_CUR,
      SEEK_END,
      MAX_SEEK,
};


struct os_stats{
               u64 swapper_invocations;
                u64 context_switches;
                u64 lw_context_switches;
                u64 ticks;
                u64 page_faults;
                u64 cow_page_faults;
                u64 syscalls;
                u64 used_memory;
                u64 num_processes;
                u64 num_vm_area;
                u64 mmap_page_faults;
                u64 user_reg_pages; // used to check copy-on-write 
};

struct os_configs{
                u64 global_mapping;
                u64 apic_tick_interval;
                u64 debug;
                u64 adv_global; 
};



extern void exit(int);
extern int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5);
extern void exit(int code);
extern long getpid();
extern long fork();
extern long cfork();
extern long vfork();
extern long get_stats();
extern long configure(struct os_configs *new_config);
extern long signal(int num, void *handler);
extern long sleep(int ticks);
extern long expand(unsigned size, int flags);
extern long clone(void* (*func) (void*), long stack_addr, void *user_arg);
extern long dump_page_table(char *address);
extern long physinfo();
extern int printf(char *format,...);
extern void* mmap(void *addr, int length, int prot, int flags);
extern int munmap(void *addr, int length);
extern int mprotect(void *addr, int length, int prot);
extern int pmap(int details);
extern long get_user_page_stats();
extern long get_cow_fault_stats();

extern int open(char * filename, int mode, ...);
extern int write(int fd, void * buf, int count);
extern int read(int fd, void * buf, int count);
extern int pipe(int fd[2]);
extern int dup(int oldfd);
extern int dup2(int oldfd, int newfd);
extern int close(int fd);
extern long lseek(int fd, long offset, int whence);
extern int ustrcmp(char * s, char * d);

//syscall to make thread ready
extern int make_thread_ready(long pid);        
//syscall to wait for thread's completion, used inside join
extern int wait_for_thread(long pid);
	
#endif
