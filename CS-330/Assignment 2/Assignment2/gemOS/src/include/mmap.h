#ifndef __MMAP_H_
#define __MMAP_H_
#include<types.h>
#include<context.h>
#include<memory.h>
#include<lib.h>
#include<entry.h>

#define MMAP_AREA_START MMAP_START
#define MMAP_AREA_END  0x7FE000000

#define MAP_FIXED 1
#define MAP_POPULATE 2

#define PROT_READ MM_RD
#define PROT_WRITE  MM_WR
#define PROT_EXEC MM_EX

long vm_area_map(struct exec_context *current, u64 addr, int length, int prot, int flags);

int vm_area_unmap(struct exec_context *current, u64 addr, int length);

int vm_area_pagefault(struct exec_context *current, u64 addr, int error_code);

int vm_area_mprotect(struct exec_context *current, u64 addr, int length, int prot);

static int vm_area_dump(struct vm_area *vm, int details);

    extern struct vm_area* create_vm_area(u64 start_addr, u64 end_addr, u32 flags);

static struct vm_area * alloc_vm_area()
{
	struct vm_area *vm = os_alloc(sizeof(struct vm_area));
	stats->num_vm_area++;
	return vm;
}

static void dealloc_vm_area(struct vm_area *vm)
{
	stats->num_vm_area--;
	os_free(vm, sizeof(struct vm_area));
}

// Function used to dump the vm_area
static int vm_area_dump(struct vm_area *vm, int details)
{
	/** TODO Have to remove -1 to get exact count */
	if(!details)
	{
		printk("VM_Area:[%d]\tMMAP_Page_Faults[%d]\n", (stats->num_vm_area - 1), stats->mmap_page_faults);
		return 0;
	}
	struct vm_area *temp = vm->vm_next; // Ommiting the dummy head nodes
	printk("\n\n\t########### \tVM Area Details \t################\n");
	printk("\tVM_Area:[%d]\t\tMMAP_Page_Faults[%d]\n\n", (stats->num_vm_area - 1), stats->mmap_page_faults);
	while(temp)
	{
		printk("\t[%x\t%x] #PAGES[%d]\t", temp->vm_start, temp->vm_end, (( temp->vm_end - temp->vm_start)/ PAGE_SIZE));
		if(temp->access_flags & PROT_READ)
			printk("R ");
		else
			printk("_ ");

		if(temp->access_flags & PROT_WRITE)
			printk("W ");
		else
			printk("_ ");

		if(temp->access_flags & PROT_EXEC)
			printk("X\n");
		else
			printk("_\n");

		temp = temp->vm_next;
	}
	printk("\n\t###############################################\n\n");
	return 0;
}

static void free_page(unsigned long pte_entry)
{
	os_pfn_free(USER_REG, (pte_entry >> PTE_SHIFT) & 0xFFFFFFFF);
}
#endif
