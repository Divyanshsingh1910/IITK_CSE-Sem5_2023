#include<ulib.h>

/*   
     XXX MUST be the first function. The OS points the USER RIP to *init_start* 
     while loading the init process. HACK remains till we have elf parsing code
     in the gemOS kernel
*/

void init_start(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
  int retval;
  retval = main(arg1, arg2, arg3, arg4, arg5);
  exit(0);
}

/*Invoke system call with no additional arguments*/
static long _syscall0(int syscall_num)
{
  asm volatile ( "int $0x80;"
                 "leaveq;"
                 "retq;"
                :::"memory");
  return 0;   /*gcc shutup!*/
}

/*Invoke system call with one argument*/

static long _syscall1(int syscall_num, long exit_code)
{
  asm volatile ( "int $0x80;"
                 "leaveq;"
                 "retq;"
                :::"memory");
  return 0;   /*gcc shutup!*/
}
/*Invoke system call with two arguments*/

static long _syscall2(int syscall_num, u64 arg1, u64 arg2)
{
  asm volatile ( "int $0x80;"
                 "leaveq;"
                 "retq;"
                :::"memory");
  return 0;   /*gcc shutup!*/
}


static long _syscall3(int syscall_num, u64 arg1, u64 arg2, u64 arg3)
{
  asm volatile ( "int $0x80;"
                 "leaveq;"
                 "retq;"
                :::"memory");
  return 0;   /*gcc shutup!*/
}

static long _syscall4(int syscall_num, u64 arg1, u64 arg2, u64 arg3, u64 arg4)
{
  asm volatile ( "int $0x80;"
                 "leaveq;"
                 "retq;"
                :::"memory");
  return 0;   /*gcc shutup!*/
}


void exit(int code)
{
  _syscall1(SYSCALL_EXIT, code); 
}

long getpid()
{
  return(_syscall0(SYSCALL_GETPID));
}

long fork()
{
  return(_syscall0(SYSCALL_FORK));
}

long cfork()
{
  return(_syscall0(SYSCALL_CFORK));
}

long vfork()
{
  return(_syscall0(SYSCALL_VFORK));
}

long physinfo()
{
  return(_syscall0(SYSCALL_PHYS_INFO));
}

long get_stats()
{
  return(_syscall0(SYSCALL_STATS));
}
long get_user_page_stats()
{
  return(_syscall0(SYSCALL_GET_USER_P));
}

long get_cow_fault_stats()
{
  return(_syscall0(SYSCALL_GET_COW_F));
}

long configure(struct os_configs *new_config)
{
  return(_syscall1(SYSCALL_CONFIGURE, (u64)new_config));
}

long dump_page_table(char *address)
{
  return(_syscall1(SYSCALL_DUMP_PTT, (u64)address));
   
}

long signal(int num, void *handler)
{
    return _syscall2(SYSCALL_SIGNAL, num, (u64)handler);
}

long sleep(int ticks)
{
    return _syscall1(SYSCALL_SLEEP, ticks);
}

long expand(unsigned size, int flags) {
	return (long)_syscall2(SYSCALL_EXPAND, size, flags);
}

long clone(void* (*func)(void*), long stack_addr, void *user_arg)
{
    return _syscall3(SYSCALL_CLONE, (u64)func, stack_addr, (u64)user_arg);
}

void* mmap(void *addr, int length, int prot, int flags)
{
    return (void*)_syscall4(SYSCALL_MMAP, (u64)addr, length, prot, flags);
}

int munmap(void *addr, int length)
{
    return (int)_syscall2(SYSCALL_MUNMAP, (u64)addr, length);
}

int mprotect(void *addr, int length, int prot)
{
    return (int)_syscall3(SYSCALL_MPROTECT, (u64)addr, length, prot);
}

int pmap(int details)
{
    return (int)_syscall1(SYSCALL_PMAP, details);
}

int open(char * filename, int flags, ...)
{
   va_list ap;
   long mode;
   va_start(ap, flags);
   mode = va_arg(ap, long);
   va_end(ap);
   if((flags & O_CREAT) == O_CREAT){     
     return _syscall3(SYSCALL_OPEN, (u64)filename, flags, mode);
   }
   else{
     return _syscall2(SYSCALL_OPEN, (u64)filename, flags);
   }
}

int read(int fd,void * buf, int count)
{
  return _syscall3(SYSCALL_READ, fd, (u64)buf, count);
}

int write(int fd, void * buf, int count)
{
  return _syscall3(SYSCALL_WRITE, fd, (u64)buf, count);
}

int pipe(int fd[2])
{
  return _syscall1(SYSCALL_PIPE, (unsigned long)fd);
}

int dup(int oldfd)
{
   return _syscall1(SYSCALL_DUP, oldfd);
}

int dup2(int oldfd, int newfd)
{
   return _syscall2(SYSCALL_DUP2, oldfd, newfd);
}

int close(int fd)
{
   return _syscall1(SYSCALL_CLOSE, fd);
}

long lseek(int fd, long offset, int whence)
{
  return _syscall3(SYSCALL_LSEEK, fd, offset, whence);
}

int make_thread_ready(long pid)
{
  return _syscall1(SYSCALL_MAKE_THREAD_READY, pid);
}

int wait_for_thread(long pid)
{
  return _syscall1(SYSCALL_WAIT_FOR_THREAD, pid);
}

static int vuprintf(char *buf,char *format,va_list args){
 int count = 0,ch,out=0;
 while((ch=format[count++])){ 
    if(ch != '%')
           buf[out++] = ch;
    else{
           ch=format[count++];
           switch(ch){
               case 'c':
                          buf[out++] = va_arg(args,int);
                          break;
               case 'd':
               case 'u':
                         {
                            int num = va_arg(args,int);
                            int tmp = num,count=0;
                            s8 chars[20];
                            if(!num){
                                  buf[out++] = '0';
                                  break;
                            }
                            if(tmp < 0){
                                  buf[out++] = '-';
                                  tmp=-tmp;
                                  num=-num;
                             }
                             while(tmp){
                                  chars[count++] = '0' + tmp % 10;
                                  tmp /= 10;
                              }
                              while(count--)
                                    buf[out++] = chars[count];
                              break;         
                         }
                       
               case 'x':
                          {
                            long num = va_arg(args, long);
                            long tmp = num, count=0;
                            s8 chars[20];
                            if(tmp < 0){
                                  buf[out++] = '-';
                                  tmp=-tmp;
                                  num=-num;
                             }
                            buf[out++] = '0';
                            buf[out++] = 'x';
                            if(!num){
                                buf[out++] = '0';
                                break;
                            }
                             while(tmp){
                                  if(tmp % 16 > 9)
                                     chars[count++] = 'A' + (tmp % 16 - 10);
                                  else
                                     chars[count++] = '0' + tmp % 16;
                                  tmp /= 16;
                              }
                              while(count--)
                                    buf[out++] = chars[count];
                              break;

                          }      
               case 's':
                          {
                            char *str=va_arg(args,char*);
                            if(str){
                                s8 p;
                                int i=0;
                                while((p=str[i++]))
                                  buf[out++] = p;
                             }
                            break;
                          }
               default:
                        buf[out++] = ch;
                        break;
           }
    }
 }   
 return out;
}


static void zero(char *ptr,int length){
    int i;
    for(i=0;i<length;++i) ptr[i] = 0;
}

int printf(char *format,...)
{
  /*XXX TODO convert static*/
    char buff[4096];
    int retval;
    va_list args;
    zero(buff,4096);
    va_start(args,format);
    retval=vuprintf(buff,format,args);
    va_end(args);
    write(1, buff, retval);
    return retval;
}

int ustrcmp(char *s, char *d){
   int ctr = 0;
   while(s[ctr] != 0 && d[ctr] != 0 && s[ctr] == d[ctr])
         ++ctr;
   if(s[ctr] == 0 && d[ctr] == 0)
        return 0;
   return -1;
}
