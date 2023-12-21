#include <types.h>
#include <mmap.h>
#include <fork.h>
#include <v2p.h>
#include <page.h>

/* 
 * You may define macros and other helper functions here
 * You must not declare and use any static/global variables 
 * */
#define KB 1024
#define bit12 0xFFF
#define bit9 0x1FF


////// Part2 Helper Function //////
///////      Start             //////

void flush_tlb(u64 addr){
    asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
    // printk("OS: TLB Flushed!!!\n");
}

int is_valid(u64 addr){
    if(addr & 0x1){
        return 1;
    }
    else return 0;
}
u64 Update_32bit_addr(u64 pgd_t,u64 pud_base){
    pgd_t = pgd_t << 52;
    pgd_t = pgd_t >> 52;
    //last ke 12 bits hi bas bache hai ab

    pud_base = pud_base << 12;

    pgd_t = pgd_t | pud_base;

    pgd_t = pgd_t | 0x19;

    return pgd_t;
}

u64 get_52bit_addr(u64 pgd_t){
    return (pgd_t & 0xFFFFFFFFFFFFF000);

}
//////        End             //////

void print_list(struct vm_area* head){
    // printk("#### printing the whole list ####\n");
    // printk("           --Head_info--          \n");
    // printk("     #ptr = %x\n",head);
    // printk("     #start = %x\n",head->vm_start);
    // printk("     #end = %x\n",head->vm_end);
    // printk("     #next_ptr = %x\n",head->vm_next);
    // printk("     #Prot = %x\n\n",head->access_flags);

    // printk("           --List_nodes--          \n");
    head = head->vm_next;
    int i = 1;
    while(head){
        // printk("     #Printing #list = %d\n",i);
        // printk("     #ptr = %x\n",head);
        // printk("     #start = %x\n",head->vm_start);
        // printk("     #end = %x\n",head->vm_end);
        // printk("     #next_ptr = %x\n",head->vm_next);
        // printk("     #Prot = %x\n\n",head->access_flags);

        i++;
        head = head->vm_next;
    }
}

struct vm_area* get_prev_node(struct vm_area* head, struct vm_area* curr){
    struct vm_area* prev = head;
    while(prev){
        if(prev->vm_next == curr){
            return prev;
        }

        prev = prev->vm_next;
    }

    return NULL;
}

////// MMAP Helper functions //////
/////      Start            //////
/*Will return the last node jiske aage allocate krna hai*/
struct vm_area* find_free_vma(struct vm_area* vm_head, int length){

    // printk("----->OS_ffv: function called!!\n");
    
    struct vm_area* vm_ptr = vm_head;
    while(vm_ptr){
        if(vm_ptr->vm_next==NULL){
            if(vm_ptr->vm_end+length>=MMAP_AREA_END){
                return NULL;
                //means that we have crosses the allocated space of mapping
            }
            // // printk("----->OS_ffv: return ho gya yahan se bro!!\n");
            return vm_ptr;
        }
        if(vm_ptr->vm_end+length<=vm_ptr->vm_next->vm_start){
            // // printk("----->OS_ffv: return ho gya yahan se bro!!\n");
            return vm_ptr;
        }

        vm_ptr = vm_ptr->vm_next;
    }
    // // printk("----->OS_ffv: NULL return ho rha hai prabhuu!!\n");
    return NULL;
}
/*Check if the following address is occupied or not*/

int Is_intersecting(u64 s1, u64 e1, u64 s2, u64 e2){
    if(e1<=s2 || e2<=s1){
        return 0;
    }
    else{
        u64 x = (s1>=s2)? s1 : s2;
        u64 y = (e1<=e2)? e1 : e2;

        if(x>=y) return 0;
        else return 1;

    }
}

int Is_occupied(struct vm_area* VMA_head,u64 addr,int length){

   struct vm_area* vm_ptr = VMA_head->vm_next;

    while(vm_ptr){
        if(Is_intersecting(vm_ptr->vm_start,vm_ptr->vm_end,addr,addr+length)){
            return 1;
        }

        vm_ptr = vm_ptr->vm_next;
    }

    return 0;

}
/*Check and Merge functions*/
void Check_and_merge(struct vm_area* VMA_head,u64 ret_addr){
    int count = 1;
    struct vm_area* vm_ptr = VMA_head->vm_next;

    while(vm_ptr && vm_ptr->vm_next){
        if(vm_ptr->vm_end == vm_ptr->vm_next->vm_start &&
                vm_ptr->access_flags == vm_ptr->vm_next->access_flags){
                    u64 temp = vm_ptr->vm_next->vm_end;

                    vm_ptr->vm_next = vm_ptr->vm_next->vm_next;
                    vm_ptr->vm_end = temp;
            }

            count++;

            vm_ptr = vm_ptr->vm_next;
    }

    if(vm_ptr && vm_ptr->vm_next==NULL){
        count++;
    }

    stats->num_vm_area = count;
}

/////      End            //////
///////////////////////////////

/*      Super function being written        */
int Mem_traversal(struct exec_context* current,u64 addr, u64* _pgd_t,
                  u64* _pud_t, u64* _pmd_t, u64* _pte_t)
{
    // // printk("OS_Mem_traverse: Called for addr: %x\n",addr);

    u64 pte_offset,pmd_offset,pud_offset,pgd_offset;

    addr = addr >> 12;

    pte_offset = addr & bit9;
    addr = addr >> 9;

    pmd_offset = addr & bit9;
    addr = addr >> 9;

    pud_offset = addr & bit9;
    addr = addr >> 9;

    pgd_offset = addr & bit9;

    u64 pgd_base = osmap(current->pgd);

    /*--------- Getting pgd entry ---------*/

    u64* pgd_t_addr = pgd_base + 8*pgd_offset; //address of the entry
    u64 pgd_t = *(pgd_t_addr);

    // // printk("OS_Mem_traverse: fetched pgd_t = %x\n",pgd_t);

    u64 pud_base; //base addr of pud table(next)
    if(!is_valid(pgd_t)){
        return 0;
    }

    /*--------- Getting pud entry ---------*/
    //get the pud tabel base addr from the pgd_t entry
    pud_base = get_52bit_addr(pgd_t);

    u64* pud_t_addr = pud_base + 8*pud_offset; //addr of the pud_t entry
    u64 pud_t = *(pud_t_addr);

    // // printk("OS_Mem_traverse: fetched pud_t = %x\n",pud_t);


    u64 pmd_base; //base addr of pmd table(next)
    if(!is_valid(pud_t)){
        return 0;
    }

    /*--------- Getting pmd entry ---------*/
    //get the pmd table base addr from the pud_t entry
    pmd_base = get_52bit_addr(pud_t);

    u64* pmd_t_addr = pmd_base + 8*pmd_offset; //addr of the pmd_t entry
    u64 pmd_t = *(pmd_t_addr);

    // // printk("OS_Mem_traverse: fetched pmd_t = %x\n",pmd_t);

    u64 pte_base; //base addr of pte table(next)
    if(!is_valid(pmd_t)){
        return 0;
    }

    /*--------- Getting pte entry ---------*/
    //get the pte table base addr from the pmd_t entry
    pte_base = get_52bit_addr(pmd_t);

    u64* pte_t_addr = pte_base + 8*pte_offset; //addr of the pte_t entry
    u64 pte_t = *(pte_t_addr);

    // // printk("OS_Mem_traverse: fetched pte_t = %x\n",pte_t);

    u64 page_base; //base addr of the physical mem frame
    if(!is_valid(pte_t)){
        // // printk("OS_Mem_traverse: pte_t is not valid, returning!\n");
        return 0;
    }  

    //updating the values
    *(_pgd_t) = pgd_t_addr;
    *(_pud_t) = pud_t_addr;
    *(_pmd_t) = pmd_t_addr;
    *(_pte_t) = pte_t_addr;

    // // printk("-->OS_Mem_traverse: Itee shri!! \n");  

    return 1;  
}

/*-----------Part2 function-----------*/
void Protection_change(struct exec_context* current,struct vm_area* vm_ptr){
    // printk("-->OS_prot_change: prot change called !! \n");

    u64 addr_ = vm_ptr->vm_start;
    while(addr_<vm_ptr->vm_end-1)
    {
        u64 addr = addr_;
        u64 pgd_t_addr,pud_t_addr,pmd_t_addr,pte_t_addr;
        //will store pointer of entries in these variables

        int exist = Mem_traversal(current,addr,&pgd_t_addr,&pud_t_addr,
                            &pmd_t_addr, &pte_t_addr);
        
        if(!exist){
            // printk("-->OS_prot_change: Maine khaayi hai khatti toffee\n");
            addr_ += 4*KB;
            continue;
        }

        u64 pte_t = *((u64*)pte_t_addr);

        // printk("OS_prot_change: pte_t before change: %x\n",pte_t);

        // changing the protection

        /*  if only VMA mein change ki need hogi toh    */
        if(vm_ptr->access_flags == PROT_READ){

            pte_t = clear_bit(pte_t,3);

        }
        else{
            pte_t = set_bit(pte_t,3);
        }


        *((u64*)pte_t_addr) = pte_t;

        // printk("OS_prot_change: pte_t after change: %x\n",pte_t);
        // printk("---------os moment------------\n");

        //flushing
        flush_tlb(addr_);
        //next iteration;
        addr_ += 4*KB;
    }

    flush_tlb(vm_ptr->vm_start);

    // printk("-->OS_prot_change: Itee shri!! \n");    
}
/*-------Unmapping physical mem-------*/
void Unmap_page(struct exec_context* current,u64 start_addr, u64 end_addr){
    // printk("-->OS_unmap_page: unmap page called !! \n");

    u64 addr_ = start_addr;
    while(addr_<end_addr)
    {
        u64 addr = addr_;
        
        u64 pgd_t_addr,pud_t_addr,pmd_t_addr,pte_t_addr;
        //will store pointer of entries in these variables

        int exist = Mem_traversal(current,addr,&pgd_t_addr,&pud_t_addr,
                            &pmd_t_addr, &pte_t_addr);
        
        if(!exist){
            
            // printk("-->OS_unmap_page: %x pe addr mapped nhi hai\n",addr);
             addr_ += 4*KB;
            continue;
        }
        else{
            // printk("-->OS_unmap_page: %x pe addr mapped hai :)\n",addr);
        }

        u64 pgd_t = *((u64*)pgd_t_addr);
        u64 pud_t = *((u64*)pud_t_addr);
        u64 pmd_t = *((u64*)pmd_t_addr);
        u64 pte_t = *((u64*)pte_t_addr);

        // printk("OS_unmap: pte_t yahan pe hai: %x\n",pte_t);

        u64 page_base;
        //base addr of the page
        page_base = get_52bit_addr(pte_t);

        u32 page_fn; //frame number of the allocated page
        page_fn = page_base >> 12;

        *((u64*) pte_t_addr) = 0;
        int put = get_pfn_refcount(page_fn);
        // printk("OS_unmap_pg: ref count is %d  %x\n",put, page_fn);
        // pte_t = clear_bit(pte_t,0);

        put = put_pfn(page_fn); //ref of allocate page frame

        if(get_pfn_refcount(page_fn) == 1){
            os_pfn_free(USER_REG, page_fn);
            // printk("put: %d\n",get_pfn_refcount(page_fn));
            // printk("OS_unmape_page: line 369\n");


            *((u64*) pte_t_addr) = pte_t;

        }

        ///flushing
        flush_tlb(addr_);

        //next iteration;
        addr_ += 4*KB;
    }

    // printk("-->OS_unmap_page: Itee shri!! \n");    
}

////// MUNMAP Helper functions //////
/////         Start           //////
struct vm_area* get_the_left_end(struct exec_context* current, struct vm_area* VMA_head, u64 addr){
    struct vm_area* vm_ptr = VMA_head;
    while(vm_ptr && vm_ptr->vm_next){
        if(vm_ptr->vm_end <= addr && addr <= vm_ptr->vm_next->vm_start){
            //vm_ptr it is
            break;
        }
        if(vm_ptr->vm_start < addr && addr < vm_ptr->vm_end){
            u64 temp = vm_ptr->vm_end;
            vm_ptr->vm_end = addr;
            Unmap_page(current,addr,temp);
            break;
        }

        vm_ptr = vm_ptr->vm_next;
    }
    //if the addr is in the last node
    if(vm_ptr->vm_next==NULL && vm_ptr->vm_start < addr && addr < vm_ptr->vm_end){
        u64 temp = vm_ptr->vm_end;
        vm_ptr->vm_end = addr;
        Unmap_page(current,addr,temp);
    }


    return  vm_ptr;   
}

struct vm_area* get_the_right_end(struct exec_context* current, struct vm_area* VMA_head, u64 right_addr){
    struct vm_area* vm_right = VMA_head;
    while(vm_right){
        if(vm_right->vm_next && (vm_right->vm_end-1)<= right_addr && (right_addr < vm_right->vm_next->vm_start)){
            vm_right = vm_right->vm_next;
            //vm_right it is
            return vm_right; 
            // break;
        }
        if(vm_right->vm_next == NULL && right_addr>=vm_right->vm_end){
            vm_right = NULL;
            return vm_right;
            // break;
        }
        if(vm_right->vm_start<=right_addr && right_addr<vm_right->vm_end){
            int node_size = vm_right->vm_end - vm_right->vm_start;
            int unmap_size = right_addr - vm_right->vm_start;

            if(node_size-unmap_size<4*KB){
                vm_right = vm_right->vm_next;
                return vm_right;
                // break;
            }
            else{
                if((node_size-unmap_size)%4*KB==0){
                    u64 temp_start = vm_right->vm_start;
                    vm_right->vm_start += unmap_size;

                    Unmap_page(current,temp_start,vm_right->vm_start);
                    return vm_right;
                    // break;
                }
                else{
                    u64 temp_start = vm_right->vm_start;
                    vm_right->vm_start += (unmap_size/(4*KB) + 1)*4*KB;

                    Unmap_page(current,temp_start,vm_right->vm_start);
                    return vm_right;
                    // break;
                }
            }
        }

        //else keep iterating
        vm_right = vm_right->vm_next;
    }

    if(vm_right == VMA_head){
        // printk("Tyranny has happend!\n");
        return NULL;
    }

    return vm_right;
}
/////          End           //////
//////////////////////////////////

/* Part3 helper functions */
u64 Is_phy_mapped(struct exec_context* ctx, u64 addr, int change_to_read)
{
    // // printk("-->OS_Is_phy_mapped: phy mapping func called !! \n");

    u64 pte_t_addr;

    int exist = Mem_traversal(ctx,addr,&pte_t_addr,&pte_t_addr,
                        &pte_t_addr, &pte_t_addr);

    if(!exist){
        // // printk("--->OS_Is_phy_mapped: No phy mapping, returning\n");
        return 0x0;
    }

    u64 pte_t = *((u64*) pte_t_addr);

    u64 page_base;

    page_base = get_52bit_addr(pte_t);

    u32 page_fn; //frame number of the allocated page
    page_fn = page_base >> 12;  

    if(change_to_read){
        get_pfn(page_fn);

        pte_t = clear_bit(pte_t,3);
        *((u64*) pte_t_addr) = pte_t;
    }
    // // printk("-->OS_Is_phy_mapped: Itee shri!! \n");  

    return pte_t;
}

void Make_child_table(struct exec_context* child_ctx, u64 addr, u64 pte_t_)
{
    // // printk("-->OS_Make_child_table:func called with parent pte_t = %x !! \n",pte_t_);
    u64 pte_offset,pmd_offset,pud_offset,pgd_offset;

    addr = addr >> 12;

    pte_offset = addr & bit9;
    addr = addr >> 9;

    pmd_offset = addr & bit9;
    addr = addr >> 9;

    pud_offset = addr & bit9;
    addr = addr >> 9;

    pgd_offset = addr & bit9;

    u64 pgd_base = osmap(child_ctx->pgd);

    /*--------- Getting pgd entry ---------*/

    u64* pgd_t_addr = pgd_base + 8*pgd_offset; //address of the entry
    u64 pgd_t = *(pgd_t_addr);

    u64 pud_base; //base addr of pud table(next)
    if(!is_valid(pgd_t)){
        pud_base = os_pfn_alloc(OS_PT_REG); //allocating new table(pud)
        pgd_t = Update_32bit_addr(pgd_t,pud_base);
        *(pgd_t_addr) = pgd_t; //updating with new entry
    }

    // // printk("-->OS_Make_child_table: pgd_t is %x\n",pgd_t);

    /*--------- Getting pud entry ---------*/
    //get the pud tabel base addr from the pgd_t entry
    pud_base = get_52bit_addr(pgd_t);


    u64* pud_t_addr = pud_base + 8*pud_offset; //addr of the pud_t entry
    u64 pud_t = *(pud_t_addr);

    u64 pmd_base; //base addr of pmd table(next)
    if(!is_valid(pud_t)){
        pmd_base = os_pfn_alloc(OS_PT_REG); //allocating new table(pmd)
        pud_t = Update_32bit_addr(pud_t,pmd_base);
        *(pud_t_addr) = pud_t; //updating with new entry
    }

    // // printk("-->OS_Make_child_table: pud_t is %x\n",pud_t);

    /*--------- Getting pmd entry ---------*/

    //get the pmd table base addr from the pud_t entry
    pmd_base = get_52bit_addr(pud_t);


    u64* pmd_t_addr = pmd_base + 8*pmd_offset; //addr of the pmd_t entry
    u64 pmd_t = *(pmd_t_addr);

    u64 pte_base; //base addr of pte table(next)
    if(!is_valid(pmd_t)){
        pte_base = os_pfn_alloc(OS_PT_REG); //allocating new table(pte)
        pmd_t = Update_32bit_addr(pmd_t,pte_base);
        *(pmd_t_addr) = pmd_t; //updating the new entry
    }

    // // printk("-->OS_Make_child_table: pmd_t is %x\n",pmd_t);

    /*--------- Getting pte entry ---------*/

    //get the pte table base addr from the pmd_t entry
    pte_base = get_52bit_addr(pmd_t);

    u64* pte_t_addr = pte_base + 8*pte_offset; //addr of the pte_t entry
    u64 pte_t = *(pte_t_addr);

    *(pte_t_addr) = pte_t_; //asigning the parent wala pte_t

    // // printk("-->OS_Make_child_table: pte_t of child is %x\n",pte_t_);
    // // printk("-->OS_Make_child_table: Itee shri!! \n");
}

///////////////////////////////////////////////
//// mprotect system call implementation. ////
/////////////////////////////////////////////
long vm_area_mprotect(struct exec_context *current, u64 addr, int length, int prot)
{
    // printk("OS_MPROTECT: called with addr  = %x and length = %d for prot = %x\n",addr,length,prot);
    // protection and flag testing
    if(current == NULL){
        return -1; //error
    }

    if(/*protection*/ !(prot == PROT_READ || prot == PROT_READ|PROT_WRITE)){                                          
        return -1;
    }

    // if vm_area is null
    if(current->vm_area == NULL){
        struct vm_area* vm  = os_alloc(sizeof(struct vm_area));
        vm->vm_start = MMAP_AREA_START;
        vm->vm_end = vm->vm_start + 4*KB;
        vm->access_flags = 0x0;
        vm->vm_next = NULL;

        current->vm_area = vm;
    }
    if(length==0){
        return 0; //success already
    }
    long end_addr = addr + length;

    struct vm_area* vm_ptr = current->vm_area->vm_next;
    while(vm_ptr){
        //case B
        // printk("-->OS_protect: While loop started!\n");

        if(vm_ptr->vm_start<=addr && end_addr<=vm_ptr->vm_end)
        {
            // printk("-->OS_protect: Case B hai \n");
            // if covers whole node toh just change the access the flag
            if(prot != vm_ptr->access_flags)
            {
                if(addr==vm_ptr->vm_start)
                { //case 4
                    
                    //now handle the page alignment scene with the end_addr
                    if((end_addr-addr)%(4*KB) != 0)
                    {
                        end_addr  = addr + ((end_addr-addr)/(4*KB)+1)*(4*KB);
                    }

                    if(end_addr==vm_ptr->vm_end){ //if covered the whole node bc
                        // printk("OS_mprotect: Ultimately Got here!\n");

                        vm_ptr->access_flags = prot;
                        Check_and_merge(current->vm_area,vm_ptr->vm_start);
                        Protection_change(current,vm_ptr);

                        return 0;
                    }
                    //else for partial coverage
                    struct vm_area* vm_prev = get_prev_node(current->vm_area,vm_ptr);
                    struct vm_area* vm_new = os_alloc(sizeof(struct vm_area));

                    //modify older node
                    vm_ptr->vm_start = end_addr;

                    //New node with changed access
                    vm_new->vm_start = addr;
                    vm_new->vm_end = end_addr;
                    vm_new->access_flags = prot;
                    vm_new->vm_next = vm_ptr;

                    //linking
                    vm_prev->vm_next = vm_new;

                    //updating the VMA count
                    stats->num_vm_area++;

                    Check_and_merge(current->vm_area,vm_new->vm_start);

                    Protection_change(current,vm_new);

                    return 0;
                }
                else{ // case2 U case3
                    if((end_addr-addr)%(4*KB)!=0){
                        end_addr = addr + ((end_addr-addr)/(4*KB)+1)*(4*KB);
                    }
                    if((vm_ptr->vm_end) == end_addr){
                        // case3
                        
                        //new node
                        struct vm_area* vm_new = os_alloc(sizeof(struct vm_area));
                        
                        vm_new->vm_start = addr;
                        vm_new->vm_end = vm_ptr->vm_end;
                        vm_new->access_flags = prot;
                        vm_new->vm_next = vm_ptr->vm_next;

                        //modify older node
                        vm_ptr->vm_end = addr;
                        vm_ptr->vm_next = vm_new;

                        //updating the VMA count
                        stats->num_vm_area++;

                        Check_and_merge(current->vm_area, vm_new->vm_start);
                        Protection_change(current,vm_new);


                        return 0;
                    }
                    else{ //case2

                        // printk("-->OS_protect: Case hai\n");

                        //creating new nodes
                        struct vm_area* vm_new1 = os_alloc(sizeof(struct vm_area));
                        struct vm_area* vm_new2 = os_alloc(sizeof(struct vm_area));

                        //initializing the new_node2
                        vm_new2->vm_start = end_addr;
                        vm_new2->vm_end = vm_ptr->vm_end;
                        vm_new2->access_flags = vm_ptr->access_flags;
                        vm_new2->vm_next = vm_ptr->vm_next;

                        //initializing the new_node1
                        vm_new1->vm_start = addr;
                        vm_new1->vm_end = end_addr;
                        vm_new1->access_flags = prot;
                        vm_new1->vm_next = vm_new2;

                        //modifying older nodes
                        vm_ptr->vm_end = addr;
                        vm_ptr->vm_next = vm_new1;

                        //updating the VMA count
                        stats->num_vm_area += 2;

                        Check_and_merge(current->vm_area,50);
                        Protection_change(current,vm_new1);
                        return 0;
                    }
                }
            }
            else return 0; //already same protection thaa, so no change!
        }
        // printk("-->OS_protect: Line346 reached!!!\n");
        //case A
        if((vm_ptr!=current->vm_area) && (vm_ptr->vm_start<=addr && addr<vm_ptr->vm_end) && (end_addr>=vm_ptr->vm_end) || (addr<vm_ptr->vm_start)){
            /*
                    ____    ______    ______    ___
                    ____|->|______|->|______|->|___
                   ---           ---            ---
                    |             |              |
                left_end      beech_wale        right_end     
                
            - vm_ptr is the left_end wala node
            - vm_rend will be the right end wala node
            - solve left_end wala case recursively
            - same with the right_end node
            - change flags of beech_wale nodes and merge accordingly
            
            */

        //    printk("Kaal ke jaal mein ghus chuke ho beta for addr = %x\n",addr);

            //handling the left_end
            if(vm_ptr->vm_start<=addr){
                long ret1 = vm_area_mprotect(current,addr,vm_ptr->vm_end-addr,prot);
            }
            
            
            //handling the right_end
            struct vm_area* vm_rend = vm_ptr->vm_next;
            while(vm_rend){
                if(vm_rend->vm_start<=end_addr && end_addr<vm_rend->vm_end){
                    break;
                }
                vm_rend = vm_rend->vm_next;
            }

            if(vm_rend!=NULL){
                long ret2 = vm_area_mprotect(current,vm_rend->vm_start,end_addr-vm_rend->vm_start,prot);
            }
            //ye length aur address pass krne mein chudne waale ho bhaii 

            //ab beech_wale ki baari
            struct vm_area* vm_mid = vm_ptr->vm_next;

            while(vm_mid!= vm_rend){
                if(vm_mid->access_flags!=prot){
                    vm_mid->access_flags=prot;
                    Check_and_merge(current->vm_area,vm_mid->vm_start);
                    Protection_change(current,vm_mid);
                    // Check_and_merge_on_right(current->vm_area,vm_mid->vm_start);
                }

                vm_mid = vm_mid->vm_next;
            }

            Check_and_merge(current->vm_area,60);

            return 0;
        }

        vm_ptr = vm_ptr->vm_next;
        // printk("-->OS_protect: vm_ptr changed to next!\n");

        if(vm_ptr == NULL){
            // printk("-->OS_protect: ptr NULL ho chuka hai bc!\n");
            return 0;
        }
        
    }

    return 0;
    // return -EINVAL;
}

////////////////////////////////////////////
//// mmap system call implementation. /////
//////////////////////////////////////////

long vm_area_map(struct exec_context *current, u64 addr, int length, int prot, int flags)
{   
    // printk("OS_MMAP: Called with addr = %x and lenght = %d\n",addr,length);
    // protection and flag testing
    if(current == NULL){
        return -1; //error
    }

    if(flags!=0 && flags != MAP_FIXED){                                          
        return -1;
    }

    if(prot!=PROT_READ && prot!= (PROT_READ | PROT_WRITE)){
        return -1;
    }

    // if(addr%(4*KB) != 0){
    //     return -1;
    // }

    if(length > 2*KB*KB || length<0){
        return -1;
    }

    if(addr && !(MMAP_AREA_START + 4*KB <= addr && addr + length < MMAP_AREA_END)){
        return -1;
    }

    // if vm_area is null
    if(current->vm_area == NULL){
        struct vm_area* vm  = os_alloc(sizeof(struct vm_area));
        vm->vm_start = MMAP_AREA_START;
        vm->vm_end = vm->vm_start + 4*KB;
        vm->access_flags = 0x0;
        vm->vm_next = NULL;

        //updating the VMA_count
        stats->num_vm_area++;

        current->vm_area = vm;
    }

    struct vm_area* VMA_head = current->vm_area;

    //Modifying the length for page alignment
    if(length%(4*KB)!=0) length = (length/(4*KB)+1)*4*KB;

    //address to return variable - {invariant across merges}
    u64 ret_addr;
    int mem_free_flag = 1; //whether the VMA area for passed addr is free or not

    if(addr==NULL && flags==MAP_FIXED){
        return -1; //error
    }

    if(addr != NULL){
        if(Is_occupied(VMA_head,addr,length)){
            mem_free_flag = 0;
        }
    }

    // printk("OS_MMAP: %x ke across space free hai? = %d\n",addr,mem_free_flag);

    if(addr == NULL || (addr!=NULL) && mem_free_flag == 0 && flags==0){
        // No hint address --> do simple assignment
        struct vm_area* vm_ptr = find_free_vma(VMA_head,length);

        struct vm_area* vm_new = os_alloc(sizeof(struct vm_area));
        vm_new->vm_start = vm_ptr->vm_end;
        vm_new->vm_end = vm_new->vm_start + length;
        vm_new->access_flags = prot;
        vm_new->vm_next = vm_ptr->vm_next;

        //updating VMA count
        stats->num_vm_area++;

        //linking
        vm_ptr->vm_next = vm_new;

        //return address
        ret_addr = (u64) vm_new->vm_start;
    }
    else if(addr!=NULL && mem_free_flag==1){
        //allocate siddha at addr and update the list(by merging)
        struct vm_area* vm_ptr = VMA_head;

        //Jis space mein allocate krne waale hain, uss se just pichla wala node
        while(vm_ptr){
            if(vm_ptr->vm_next==NULL){
                break; 
                //vm_ptr it is
            }

            if(vm_ptr->vm_end<=addr && addr<vm_ptr->vm_next->vm_start){
                break;
                //vm_ptr it is
            }

            vm_ptr = vm_ptr->vm_next;
        }

        struct vm_area* vm_new = os_alloc(sizeof(struct vm_area));
        vm_new->vm_start = addr;
        vm_new->vm_end = vm_new->vm_start + length;
        vm_new->access_flags = prot;
        vm_new->vm_next = vm_ptr->vm_next;

        //linking
        vm_ptr->vm_next = vm_new;

        //updating the VMA count
        stats->num_vm_area++;

        //updating address
        ret_addr = (u64) vm_new->vm_start;
    }
    else if(addr!=NULL && mem_free_flag == 0 && flags==MAP_FIXED){
        return -1;
    }

    // // printk("----->OS_map: Check left was called!\n");
    Check_and_merge(VMA_head,ret_addr);
    // // printk("----->OS_map: Check right was called!\n");
    // Check_and_merge_on_right(VMA_head,ret_addr);
    // // printk("----->OS_map: Check right return ho gya!\n");

    //updating the VMA count
    int count = 0;
    VMA_head = current->vm_area;
    while(VMA_head){
        count++;
        VMA_head = VMA_head->vm_next;
    }
    stats->num_vm_area = count;

    return ret_addr;

    return -EINVAL;
}

/////////////////////////////////////////////
//// munmap system call implementation. ////
///////////////////////////////////////////

long vm_area_unmap(struct exec_context *current, u64 addr, int length)
{
    // printk("OS_UNMAP: called with addr  = %x and length = %d\n",addr,length);
    //Checking
    if(current == NULL){
        return -1; //error
    }
    // if vm_area is null
    if(current->vm_area == NULL){
        struct vm_area* vm  = os_alloc(sizeof(struct vm_area));
        vm->vm_start = MMAP_AREA_START;
        vm->vm_end = vm->vm_start + 4*KB;
        vm->access_flags = 0x0;
        vm->vm_next = NULL;

        current->vm_area = vm;
    }
    struct vm_area* VMA_head = current->vm_area;

    //check if the space to be unmapped lies inside a single node
    struct vm_area* vm_curr = VMA_head;
    int flag_single_node = 0;
    while(vm_curr){
        if(vm_curr->vm_start<=addr && (vm_curr->vm_end)>=(addr+length)){
            // printk("OS_UNMAP: here we are at 1025\n");
            flag_single_node = 1;
            long end_addr = addr + length;
            if((length%(4*KB)!=0)){
                end_addr = addr + (length/(4*KB) + 1)*4*KB;
            }
            // printk("OS_UNMAP: addr = %x and end_addr = %x\n",addr,end_addr);

            if(addr==vm_curr->vm_start && end_addr==vm_curr->vm_end){
                struct vm_area* vm_previous = get_prev_node(current->vm_area,vm_curr);
                vm_previous->vm_next = vm_curr->vm_next;

                Unmap_page(current,vm_curr->vm_start,vm_curr->vm_end);
            }
            else if(end_addr == vm_curr->vm_end){
                Unmap_page(current,addr,end_addr);
                vm_curr->vm_end = addr;
            }
            else if(addr==vm_curr->vm_start){
                Unmap_page(current,addr,end_addr);
                vm_curr->vm_start = end_addr;
            }
            else{
                //New node
                struct vm_area* vm_new = os_alloc(sizeof(struct vm_area));
                vm_new->vm_start = end_addr;
                vm_new->vm_end = vm_curr->vm_end;
                vm_new->access_flags = vm_curr->access_flags;
                vm_new->vm_next = vm_curr->vm_next;

                //old node modification
                vm_curr->vm_end = addr;
                vm_curr->vm_next = vm_new;

                Unmap_page(current,addr,end_addr);
            }

            break;
        }
        vm_curr = vm_curr->vm_next;
    }
    if(!flag_single_node){

        // printk("OS_UNMAP: Yahan tk toh aagye atleast\n");

        //finding the left end node
        struct vm_area* vm_left = get_the_left_end(current,VMA_head,addr);
        if(vm_left==NULL){
            // printk("OS_UNMAP: Left end finder returned NULL\n");
        }
        // printk("OS_UNMAP: vm_left->start = %x\n",vm_left->vm_start);
        // printk("OS_UNMAP: vm_left->end = %x\n",vm_left->vm_end);

        //finding the right end node
        //here length is not page aligned coz that's correct :)
        u64 right_addr = addr + length; //excluding this boundary addr value

        struct vm_area* vm_right = get_the_right_end(current,VMA_head,right_addr);
        if(vm_right==NULL){
            // printk("OS_UNMAP: ##Chill ho skta hai ye--Right end finder returned NULL\n");
        }

        // printk("OS_UNMAP: vm_right->start = %x\n",vm_right->vm_start);
        // printk("OS_UNMAP: vm_right->end = %x\n",vm_right->vm_end);
        
        struct vm_area* vm_itr;

        // if(vm_left->vm_next){
        vm_itr = vm_left->vm_next;
        // }

        while(vm_itr && vm_itr!=vm_right){
            Unmap_page(current,vm_itr->vm_start,vm_itr->vm_end);
            vm_itr = vm_itr->vm_next;
        }

        //updating the link
        vm_left->vm_next = vm_right;
    }

    //updating the VMA count
    int count = 0;
    VMA_head = current->vm_area;
    while(VMA_head){
        count++;
        VMA_head = VMA_head->vm_next;
    }
    stats->num_vm_area = count;

    return 0;
    return -EINVAL;
}



/**
 * Function will invoked whenever there is page fault for an address in the vm area region
 * created using mmap
 */
/*-----------------pagefault helper functions [Start]-----------------*/
int Check_VMA_allocation(struct vm_area* head,u64 addr){
    head = head->vm_next;
    while(head){
        if(head->vm_start<=addr && addr<head->vm_end){
            return head->access_flags;
        }
        head = head->vm_next;
    }
    return -1;
}

void Allocate_phy_mem(struct exec_context* current,u64 addr, int PROT){
    // printk("-->OS_allocate: Allocate func called !! \n");
    u64 pte_offset,pmd_offset,pud_offset,pgd_offset;

    addr = addr >> 12;

    pte_offset = addr & bit9;
    addr = addr >> 9;

    pmd_offset = addr & bit9;
    addr = addr >> 9;

    pud_offset = addr & bit9;
    addr = addr >> 9;

    pgd_offset = addr & bit9;

    u64 pgd_base = osmap(current->pgd);

    int pfn;
    /*--------- Getting pgd entry ---------*/
    pfn = get_pfn(pgd_base>>12);

    u64* pgd_t_addr = pgd_base + 8*pgd_offset; //address of the entry
    u64 pgd_t = *(pgd_t_addr);

    u64 pud_base; //base addr of pud table(next)
    if(!is_valid(pgd_t)){
        pud_base = os_pfn_alloc(OS_PT_REG); //allocating new table(pud)
        pgd_t = Update_32bit_addr(pgd_t,pud_base);
        *(pgd_t_addr) = pgd_t; //updating with new entry
    }

    // printk("-->OS_allocate: pgd_t is %x\n",pgd_t);

    /*--------- Getting pud entry ---------*/
    //get the pud tabel base addr from the pgd_t entry
    pud_base = get_52bit_addr(pgd_t);

    u64* pud_t_addr = pud_base + 8*pud_offset; //addr of the pud_t entry
    u64 pud_t = *(pud_t_addr);

    u64 pmd_base; //base addr of pmd table(next)
    if(!is_valid(pud_t)){
        pmd_base = os_pfn_alloc(OS_PT_REG); //allocating new table(pmd)
        pud_t = Update_32bit_addr(pud_t,pmd_base);
        *(pud_t_addr) = pud_t; //updating with new entry
    }

    // printk("-->OS_allocate: pud_t is %x\n",pud_t);

    /*--------- Getting pmd entry ---------*/

    //get the pmd table base addr from the pud_t entry
    pmd_base = get_52bit_addr(pud_t);

    u64* pmd_t_addr = pmd_base + 8*pmd_offset; //addr of the pmd_t entry
    u64 pmd_t = *(pmd_t_addr);

    u64 pte_base; //base addr of pte table(next)
    if(!is_valid(pmd_t)){
        pte_base = os_pfn_alloc(OS_PT_REG); //allocating new table(pte)
        pmd_t = Update_32bit_addr(pmd_t,pte_base);
        *(pmd_t_addr) = pmd_t; //updating the new entry
    }

    // printk("-->OS_allocate: pmd_t is %x\n",pmd_t);

    /*--------- Getting pte entry ---------*/

    //get the pte table base addr from the pmd_t entry
    pte_base = get_52bit_addr(pmd_t);

    u64* pte_t_addr = pte_base + 8*pte_offset; //addr of the pte_t entry
    u64 pte_t = *(pte_t_addr);

    u64 page_base; //base addr of the physical mem frame
    if(!is_valid(pte_t)){
        page_base = os_pfn_alloc(USER_REG); //allocating the physical mem frame
        pte_t = Update_32bit_addr(pte_t,page_base);

        //setting the protection
        if(PROT == PROT_READ){
            pte_t = pte_t >> 4;
            pte_t = pte_t << 4;
            pte_t = pte_t | 0x1;
        }

        *(pte_t_addr) = pte_t; //updating the new entry
    }

    // printk("-->OS_allocate: pte_t is %x\n",pte_t);
    // printk("-->OS_page_fault: Itee shri!! \n");
}
/*-----------------pagefault helper functions [End]------------------*/

long vm_area_pagefault(struct exec_context *current, u64 addr, int error_code)
{
    // printk("-->OS_page_fault: Called with addr = %x and error_code = %x \n",addr,error_code);
    //Checking
    if(current == NULL){
        return -1; //error
    }
    // if vm_area is null
    if(current->vm_area == NULL){
        struct vm_area* vm  = os_alloc(sizeof(struct vm_area));
        vm->vm_start = MMAP_AREA_START;
        vm->vm_end = vm->vm_start + 4*KB;
        vm->access_flags = 0x0;
        vm->vm_next = NULL;

        current->vm_area = vm;
    }

    struct vm_area* VMA_head = current->vm_area;

    // // printk("-->OS_page_fault: Reached line 804\n");

    int protection = Check_VMA_allocation(VMA_head,addr);
    // printk("OS_page_fault: protection is %x\n",protection);
    // // printk("-->OS_page_fault: Reached line 807\n");
    if(protection == -1){
        return -1;
    }

    if(protection == 0x1 && error_code == 0x7){
        return -1;
    }
    //Invalid access: VMA not allocated

    if(protection == PROT_READ && error_code == 0x6) return -1;
    //Invalided access: writing access on a read-only page

    if(error_code==0x7){
        // return -1;
        return handle_cow_fault(current,addr,protection);
    }

    Allocate_phy_mem(current,addr,protection);
    return 1;
}

/**
 * cfork system call implemenations
 * The parent returns the pid of child process. The return path of
 * the child process is handled separately through the calls at the 
 * end of this function (e.g., setup_child_context etc.)
 */

long do_cfork(){
    // printk("OS_cfork: Function is called!\n");

    u32 pid;
    struct exec_context *new_ctx = get_new_ctx();
    struct exec_context *ctx = get_current_ctx();
     /* Do not modify above lines
     * 
     * */   
     /*--------------------- Your code [start]---------------*/
     //creating the child VMA list
     struct vm_area* vm_head = os_alloc(sizeof(struct vm_area));
     new_ctx->vm_area = vm_head;

     struct vm_area* vm_ptr = ctx->vm_area;

     while(vm_ptr){
        vm_head->vm_start = vm_ptr->vm_start;
        vm_head->vm_end = vm_ptr->vm_end;
        vm_head->access_flags = vm_ptr->access_flags;
        vm_head->vm_next = NULL;

        if(vm_ptr->vm_next){
            struct vm_area* vm_new = os_alloc(sizeof(struct vm_area));

            vm_head->vm_next = vm_new;
        }

        vm_ptr = vm_ptr->vm_next;
        vm_head = vm_head->vm_next;
     }

     // printk("OS_cfork: VMA areas are copied!\n");

    u32 child_pgd = os_pfn_alloc(OS_PT_REG);
    new_ctx->pgd = child_pgd;

    // printk("os_cfork:pgd allocated!\n");


     // printk("\n---------------MM segment copying-------------\n\n");

     //copying the mm_segment
     for(int i=0; i<MAX_MM_SEGS; i++){
        // printk("\n###--------%dth of %d sgs---------###\n",i,MAX_MM_SEGS);
        new_ctx->mms[i] = ctx->mms[i];
        //copying the PTE for this as well
        u64 addr = ctx->mms[i].start;
        u64 end_addr;

        if(i!=3) end_addr = ctx->mms[i].next_free;
        else end_addr = ctx->mms[i].end;


        while(addr<end_addr){
            int change_to_read = 1;
            u64 pte_t = Is_phy_mapped(ctx,addr,change_to_read);
            //  // printk("OS_cfork: pte_t returned is: %x\n",pte_t);
            if(pte_t){
                // // printk("os_cfork: PTE: pte_t enter: %x\n",pte_t);
                
                Make_child_table(new_ctx,addr,pte_t);

                flush_tlb(addr);

                // u64 pte_t_addr;

                // int ret = Mem_traversal(new_ctx,addr,&pte_t_addr,&pte_t_addr,&pte_t_addr,&pte_t_addr);

                // u64 temp = *((u64*)pte_t_addr);

                // // printk("os_cfork: PTE: pte_t out for child: %x\n",pte_t);
            }
            // // printk("\n");

            addr += 4*KB;
        }
     }
    
     // printk("OS_cfork: MM Segments are copied!\n");

     // copying other attributes

     memcpy(new_ctx->name,ctx->name,CNAME_MAX);
     new_ctx->regs = ctx->regs;
     new_ctx->pending_signal_bitmap = ctx->pending_signal_bitmap;

     for(int i=0; i<MAX_SIGNALS; i++){
        new_ctx->sighandlers[i] = ctx->sighandlers[i];
     }

     new_ctx->ticks_to_sleep = ctx->ticks_to_sleep;
     new_ctx->alarm_config_time = ctx->alarm_config_time;
     new_ctx->ticks_to_alarm = ctx->ticks_to_alarm;

     new_ctx->ctx_threads = ctx->ctx_threads;

     for(int i=0; i<MAX_OPEN_FILES; i++){
        new_ctx->files[i] = ctx->files[i];
     }
     
    //  Allocating the page table memory to child
    // printk("OS_cfork: Ctx attributes copied are copied!\n");

    vm_ptr = ctx->vm_area->vm_next;

    while(vm_ptr){
        u64 addr = vm_ptr->vm_start;
        u64 end_addr = vm_ptr->vm_end;
        while(addr<end_addr){
            int change_to_read = 1;
            u64 pte_t = Is_phy_mapped(ctx,addr,change_to_read);
            // // printk("OS_cfork: pte_t returned is: %x\n",pte_t);
            if(pte_t){

                // // printk("os_cfork: PTE: pte_t enter: %x\n",pte_t);
                
                Make_child_table(new_ctx,addr,pte_t);

                // // printk("OS_handle_cow_fault: After copying child table")

                flush_tlb(addr);

                // u64 pte_t_addr;

                // int ret = Mem_traversal(new_ctx,addr,&pte_t_addr,&pte_t_addr,&pte_t_addr,&pte_t_addr);

                // u64 temp = *((u64*)pte_t_addr);

                // // printk("os_cfork: PTE: pte_t out for child: %x\n",pte_t);
            }
            else{
                // // printk("&&&&OS_cfork: Continue to next addr\n");
            }

            addr += 4*KB;
        }

        vm_ptr = vm_ptr->vm_next;
    }

    // printk("OS_cfork: Page Table Entries are copied!\n");

     //Copying the context
     pid = new_ctx->pid;
     new_ctx->ppid = ctx->pid;
     new_ctx->type = ctx->type;
     new_ctx->state = ctx->state;
     new_ctx->used_mem = ctx->used_mem;

     pid = new_ctx->pid;

     // printk("OS_csfork: yahan tk chal gya\n"); 
     /*--------------------- Your code [end] ----------------*/
    
     /*
     * The remaining part must not be changed
     */
    copy_os_pts(ctx->pgd, new_ctx->pgd);
    do_file_fork(new_ctx);
    setup_child_context(new_ctx);
    return pid;
}



/* Cow fault handling, for the entire user address space
 * For address belonging to memory segments (i.e., stack, data) 
 * it is called when there is a CoW violation in these areas. 
 *
 * For vm areas, your fault handler 'vm_area_pagefault'
 * should invoke this function
 * */

long handle_cow_fault(struct exec_context *current, u64 vaddr, int access_flags)
{
    // printk("-->OS_handle_cow_fault: function called !! \n");

    u64 pgd_t_addr,pud_t_addr,pmd_t_addr,pte_t_addr;
    //will store pointer of entries in these variables

    int exist = Mem_traversal(current,vaddr,&pgd_t_addr,&pud_t_addr,
                        &pmd_t_addr, &pte_t_addr);
    
    if(!exist){
        // printk("-->OS_handle_cow_fault: Maine khaayi hai khatti toffee\n");
    }   

    u64 pte_t = *((u64*)pte_t_addr);

    u64 page_base = get_52bit_addr(pte_t);

    u32 page_fn;
    page_fn = page_base >> 12;


    int put = get_pfn_refcount(page_fn); //old page 
    // printk("Os_handle_cow_fault: old ref count is: %d \n",put);

    if(put>1){
        put = put_pfn(page_fn);

        u32 new_pfn = os_pfn_alloc(USER_REG);

        // printk("Os_cow_fault: pte_t yahan pe aayi: %x\n",pte_t);
        // printk("Os_cow_fault: Old pfn: %x and New pfn: %x\n",page_fn,new_pfn);

        pte_t = Update_32bit_addr(pte_t,new_pfn);

        //copying contents
        memcpy(osmap(new_pfn),osmap(page_fn),4*KB);

        // if(access_flags == PROT_READ){
            
        //     pte_t = pte_t >> 4;
        //     pte_t = pte_t << 4;
        //     pte_t = pte_t | 0x1;
        // }

        *((u64*)pte_t_addr) = pte_t; //updating the new entry

        // // printk("OS_handle_cow_fault: pte_t exited: %x\n",pte_t);
    }
    else{
         // printk("OS_handle_cow_fault: pte_t entered: %x with flag = %x\n",pte_t,access_flags);
        if(access_flags == PROT_READ | PROT_WRITE){
            pte_t = pte_t >> 4;
            pte_t = pte_t << 4;
            pte_t = pte_t | 0x9;

            *((u64*)pte_t_addr) = pte_t; //updating the access 
        }
        //   // printk("OS_handle_cow_fault: pte_t exited: %x\n",pte_t);
    }


    // printk("-->OS_handle_cow_fault: Itee shri!! \n");  
  return 1;
}

