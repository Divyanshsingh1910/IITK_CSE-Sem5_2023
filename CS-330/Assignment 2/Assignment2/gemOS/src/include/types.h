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

struct node{
	u64 value;
	union{
		struct node *next;
		struct node *prev;
	};
};

struct list{
	struct node *head;
	struct node *tail;
	u64 size;
};

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
#endif
