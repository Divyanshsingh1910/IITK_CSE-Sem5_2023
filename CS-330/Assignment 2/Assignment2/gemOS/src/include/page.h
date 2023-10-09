#include<types.h>

struct pfn_info{
	u8 refcount;
};

struct pfn_info_list{
	void * start;
	void * end;
};

struct pfn_info_list list_pfn_info;

struct pfn_info * get_pfn_info(u32 index);

extern void set_pfn_info(u32 index);
extern void reset_pfn_info(u32 index);


void increment_pfn_info_refcount(struct pfn_info * p);

void decrement_pfn_info_refcount(struct pfn_info * p);

extern u8 get_pfn_info_refcount(struct pfn_info *p);
