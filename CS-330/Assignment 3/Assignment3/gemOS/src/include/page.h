#ifndef __PAGE_H_
#define __PAGE_H
#include<types.h>

struct pfn_info{
    s8 refcount;
};

struct pfn_info_list{
    void *start;
    void *end;
};

extern void init_pfn_info(u64);
extern struct pfn_info* get_pfn_info(u32 index);
extern void set_pfn_info(u32 index);
extern void reset_pfn_info(u32 index);

extern s8 get_pfn_refcount(u32 pfn);
extern s8 get_pfn(u32 pfn);
extern s8 put_pfn(u32 pfn);
#endif
