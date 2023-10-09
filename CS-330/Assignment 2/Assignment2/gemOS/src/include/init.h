#ifndef __INIT_H_
#define __INIT_H_
#include <types.h>
#include <context.h>
#include <entry.h>

struct init_args{
	u64 rdi;
	u64 rsi;
	u64 rcx;
	u64 rdx;
	u64 r8;
};

extern int exec_init (struct exec_context *, struct init_args *);
extern void init_start(u64, u64, u64, u64, u64);

#endif
