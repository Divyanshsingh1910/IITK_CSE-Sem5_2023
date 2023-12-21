#ifndef __IDT_H_
#define __IDT_H_

#include <types.h>
#include <context.h>

#pragma pack(1)


#define IDT_DESC_TYPE_UTRAP    0xEF
#define IDT_DESC_TYPE_KTRAP    0x8F
#define IDT_DESC_TYPE_INT_SW   0xEE
#define IDT_DESC_TYPE_INT_HW   0x8E
#define NUM_IDT_ENTRY 256

#define FAULT_DIVZERO 0x0
#define FAULT_GP 0xD
#define FAULT_PF 0xE
#define SYSCALL_IDT 0x80

/*APIC */
#define APIC_TIMER 32
#define IRQ_SPURIOUS 255

struct idt_entry{
                               u16 offset_low;
                               u16 segment;
                               u8 ist;   /*Only the 3-LSBs*/
                               u8 flags_type;
                               u16 offset_mid;
                               u32 offset_high;
                               u32 unused;
};

struct IDTR{
               u16 limit;
               u64 base;
};

struct gdt_entry{
                    u16 limit_low;
                    u16 base_low;
                    u8  base_mid;
                    u8  ac_byte;
                    u8 limit_high:4,
                       flags:4;
                    u8  base_high;     
}; 

struct tss{
             u32 reserved;
             u32 rsp0_low;
             u32 rsp0_high;
             u32 unused[6];
             u32 ist1_low;
             u32 ist1_high;
             char rest[0];
};

extern void setup_idt();        
extern void setup_gdt_tss(struct IDTR *);
extern void set_tss_stack_ptr(struct exec_context *);
#endif
