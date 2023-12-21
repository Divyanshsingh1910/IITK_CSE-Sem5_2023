#ifndef __MMAP_H_
#define __MMAP_H_
#include<types.h>
#include<context.h>
#include<memory.h>
#include<lib.h>
#include<entry.h>

#define MMAP_AREA_START MMAP_START
#define MMAP_AREA_END  0x7FE000000

#define MAP_FIXED 0x1
#define MAP_POPULATE 0x2
#define MAP_TH_PRIVATE 0x4

#define PROT_NONE   0
#define PROT_READ   MM_RD
#define PROT_WRITE  MM_WR
#define PROT_EXEC   MM_EX
#define PROT_SIB_NONE TP_SIBLINGS_NOACCESS 
#define PROT_SIB_READ TP_SIBLINGS_RDONLY
#define PROT_SIB_RW TP_SIBLINGS_RDWR

extern int vm_area_dump(struct vm_area *vm, int details);
extern long vm_area_map(struct exec_context *current, u64 addr, int length, int prot, int flags);
extern long vm_area_unmap(struct exec_context *current, u64 addr, int length);
extern long vm_area_pagefault(struct exec_context *current, u64 addr, int error_code);
extern long vm_area_mprotect(struct exec_context *current, u64 addr, int length, int prot);
extern struct vm_area* create_vm_area(u64 start_addr, u64 end_addr, u32 flags);
extern void do_vma_exit(struct exec_context *);
extern long do_expand(struct exec_context *ctx, u64 size, int segment_t);
extern long do_shrink(struct exec_context *ctx, u64 size, int segment_t);

#endif
