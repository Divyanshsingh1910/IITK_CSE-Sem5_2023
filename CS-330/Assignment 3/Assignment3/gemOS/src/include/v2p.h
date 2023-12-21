#ifndef __V2P_H__
#define __V2P_H__

/*Do not modify anything below*/
extern long vm_area_pagefault(struct exec_context *current, u64 addr, int error_code);
extern long handle_cow_fault(struct exec_context *current, u64 vaddr, int access_flags);

#endif
