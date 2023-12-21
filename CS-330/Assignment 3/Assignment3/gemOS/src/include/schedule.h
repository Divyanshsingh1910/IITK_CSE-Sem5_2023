#ifndef __SCHEDULE_H_
#define __SCHEDULE_H_
enum signals{
               SIGSEGV,
               SIGFPE,
               SIGALRM,
               MAX_SIGNALS
};

enum process_state{
                       UNUSED,
                       NEW,
                       READY,
                       RUNNING,
                       WAITING,
                       EXITING,
                       MAX_STATE
};
  
extern long invoke_sync_signal(int signo, u64 *ustackp, u64 *urip);
extern struct exec_context *pick_next_context(struct exec_context *ctx); 
extern void schedule(struct exec_context *new_ctx); 
extern int handle_timer_tick();
#endif
