#ifndef __CONTEXT_H_
#define __CONTEXT_H_

#include<types.h>
#include<schedule.h>
#include <file.h>

#define CNAME_MAX 64

#define MAX_PROCESSES 32
#define MAX_OPEN_FILES 32

enum{
           EXEC_CTX_BOOT,
           EXEC_CTX_OS,
           EXEC_CTX_USER,
           EXEC_CTX_USER_TH,
           MAX_CTX
};
enum{
          MM_SEG_CODE,
          MM_SEG_RODATA,
          MM_SEG_DATA,
          MM_SEG_STACK,
          MAX_MM_SEGS
};

#define MM_RD 0x1
#define MM_WR 0x2
#define MM_EX 0x4


#define CODE_START       0x100000000  
#define RODATA_START     0x140000000 
#define DATA_START       0x180000000
#define MMAP_START       0x180200000
#define STACK_START      0x800000000 


#define MAX_STACK_SIZE   0x1000000





struct mm_segment{
                   unsigned long start;
                   unsigned long end;
                   unsigned long next_free; 
                   u32 access_flags;   /*R=1, W=2, X=4, S=8*/ 
};

struct vm_area{
        unsigned long vm_start; // Starting address of the vm_area
        unsigned long vm_end;   // Ending address of the vm_area
        u32 access_flags;  // Access rights or protection flags of the vm_area/*R=1, W=2, X=4*/
        struct vm_area *vm_next; // Pointer to the next vm_area
};


struct user_regs{
                    u64 r15;
                    u64 r14;
                    u64 r13;
                    u64 r12;
                    u64 r11;
                    u64 r10;
                    u64 r9;
                    u64 r8;
                    u64 rbp;
                    u64 rdi;
                    u64 rsi;
                    u64 rdx;
                    u64 rcx;
                    u64 rbx;
                    u64 rax;
                    u64 entry_rip;  
                    u64 entry_cs;  
                    u64 entry_rflags;
                    u64 entry_rsp;
                    u64 entry_ss;
};

struct exec_context{
             u32 pid;
             u32 ppid;
             u8 type;
             u8 state;     /*Can be any of the states mentioned in schedule.h*/
             u16 used_mem;
             u32 pgd;     /*CR3 should point to this PFN when schedulled*/
             u32 os_stack_pfn;  /*Must be unique for every context*/
             u64 os_rsp;
             struct mm_segment mms[MAX_MM_SEGS];
             struct vm_area * vm_area;
             char name[CNAME_MAX];
             struct user_regs regs;          /*Saved copy of user registers*/
             u32 pending_signal_bitmap;      /*Pending signal bitmap*/
             void* sighandlers[MAX_SIGNALS]; /*Signal handler pointers to functions (in user space)*/
             u32 ticks_to_sleep;    /*Remaining ticks before sleep expires*/
             u32 alarm_config_time;   /*Alarm ticks set by alarm() system call*/
             u32 ticks_to_alarm;   /*Remaining ticks before raising SIGALRM*/
             struct file* files[MAX_OPEN_FILES]; /*To keep record of openfiles */
             struct ctx_thread_info *ctx_threads;
};

extern struct exec_context* create_context(char *, u8);
extern void release_context(struct exec_context *ctx);
extern struct exec_context *get_current_ctx(void);
extern void set_current_ctx(struct exec_context *);
extern struct exec_context *get_ctx_by_pid(u32 pid);
extern void load_swapper();
extern void init_swapper();
extern struct exec_context *get_ctx_list();
extern struct exec_context *get_new_ctx(); 
extern void do_cleanup(void);

#endif
