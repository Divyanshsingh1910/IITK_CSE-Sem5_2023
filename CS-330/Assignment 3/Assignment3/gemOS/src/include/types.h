#ifndef _DEBIX_TYPE_H_
   #define _DEBIX_TYPE_H_
#define NULL (void *)0
typedef unsigned int u32;
typedef 	 int s32;
typedef unsigned short u16;
typedef int      short s16;
typedef unsigned char u8;
typedef char	      s8;

#if __x84_64__
typedef unsigned long u64;
typedef long s64
#else
typedef unsigned long long u64;
typedef long long s64;
#endif

#define pause() do{\
                    __asm__ volatile( \
                                       "pause;" \
                                    );\
                }while(0);

#define gassert(x, msg) do{ \
                            if(!(x)){\
                                     printk("BUG! Assert failure in %s: %s:%d\n %s\n", __FILE__, __func__, __LINE__,msg);\
                                    __asm__ volatile (".word 0x040F; .word 0x0021;" : : "D"(0) :);\
                           }\
                          }while(0);

#define OS_BUG(msg) do{\
                          printk("OS BUG! %s\n", msg); \
                           __asm__ volatile (".word 0x040F; .word 0x0021;" : : "D"(0) :);\
	            }while(0);

/*Multiboot information*/

#define MB_HANDSHAKE 0x2badb002


/*Bios reported memory types*/

#define MB_BIOS_AVAILABLE 1
#define MB_BIOS_RESERVED 2
#define MB_BIOS_ACPI_RECLAIM 3
#define MB_BIOS_HIBERNATION 4
#define MB_BIOS_BAD 5

struct multiboot_memory_map
     {
       u32 size;
       u64 addr;
       u64 len;
       u32 type;
     } __attribute__((packed));

struct multiboot_info{
                     u32 flags;
                     u32 memlow;
                     u32 memhigh;
                     u32 bootdev;
                     u32 cmdline;
                     u32 num_modules;
                     u32 mods_base;
                     u8 syms[12];
                     u32 memory_map_length;
                     u32 memory_map_addr;
                     char padd[0];
};
static inline void outb(u16 port, u8 value){
   asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
} 

static inline void outw(u16 port, u16 value){
   asm volatile ("outw %1, %0" : : "dN" (port), "a" (value));
} 

static inline u8  inb(u16 port)
{
   u8 ret;
   asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}
static inline u16 inw(u16 port)
{
   u16 ret;
   asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
} 

//Error numbers. must be used by appending a unary ,minus

#define EINVAL 1
#define EAGAIN 2
#define EBUSY 3
#define EACCES 4
#define ENOMEM 5


#define MAX_WRITE_LEN 1024
#define MAX_EXPAND_PAGES 1024

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
                u64 file_objects;
};
extern struct os_stats *stats;
struct os_configs{
                u64 global_mapping;
                u64 apic_tick_interval;
                u64 debug;
                u64 adv_global; 
};

extern struct os_configs *config;

#endif
