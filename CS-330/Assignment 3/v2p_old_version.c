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

void print_list(struct vm_area* head){
    printk("#### printing the whole list ####\n");
    printk("           --Head_info--          \n");
    printk("     #ptr = %x\n",head);
    printk("     #start = %x\n",head->vm_start);
    printk("     #end = %x\n",head->vm_end);
    printk("     #next_ptr = %x\n",head->vm_next);
    printk("     #Prot = %x\n\n",head->access_flags);

    printk("           --List_nodes--          \n");
    head = head->vm_next;
    int i = 1;
    while(head){
        printk("     #Printing #list = %d\n",i);
        printk("     #ptr = %x\n",head);
        printk("     #start = %x\n",head->vm_start);
        printk("     #end = %x\n",head->vm_end);
        printk("     #next_ptr = %x\n",head->vm_next);
        printk("     #Prot = %x\n\n",head->access_flags);

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

    printk("----->OS_ffv: function called!!\n");
    
    struct vm_area* vm_ptr = vm_head;
    while(vm_ptr){
        if(vm_ptr->vm_next==NULL){
            if(vm_ptr->vm_end+length>=MMAP_AREA_END){
                return NULL;
                //means that we have crosses the allocated space of mapping
            }
            // printk("----->OS_ffv: return ho gya yahan se bro!!\n");
            return vm_ptr;
        }
        if(vm_ptr->vm_end+length<=vm_ptr->vm_next->vm_start){
            // printk("----->OS_ffv: return ho gya yahan se bro!!\n");
            return vm_ptr;
        }

        vm_ptr = vm_ptr->vm_next;
    }
    // printk("----->OS_ffv: NULL return ho rha hai prabhuu!!\n");
    return NULL;
}
/*Check if the following address is occupied or not*/
int Is_occupied(struct vm_area* VMA_head,u64 addr,int length){

    struct vm_area* vm_ptr = VMA_head;
    while(vm_ptr){
        if(vm_ptr->vm_next==NULL){
            if(vm_ptr->vm_end<=addr && (addr+length)<=MMAP_AREA_END){
                return 1;
                //kind of occupied hi hai kyunki allocate nhi kr skte
            }
            else return 0;
        }

        if(vm_ptr->vm_end<=addr && (addr+length)<vm_ptr->vm_next->vm_start){
            return 0;
        } //there is a benefit if we do it this way ~ which is actually kinda interesting

        vm_ptr=vm_ptr->vm_next;
    }
    return 1;
}
/*Check and Merge functions*/
void Check_and_merge_on_left(struct vm_area* VMA_head,u64 ret_addr){
    struct vm_area* vm_ptr = VMA_head;
    while(vm_ptr && vm_ptr->vm_next){
        if(vm_ptr->vm_next->vm_start == ret_addr){
            break;
        }
        vm_ptr = vm_ptr->vm_next;
    }
    if(vm_ptr == VMA_head){
        return;
    }
    else{
        if(vm_ptr->access_flags == vm_ptr->vm_next->access_flags && vm_ptr->vm_end==vm_ptr->vm_next->vm_start){
            struct vm_area* temp = vm_ptr->vm_next;
            vm_ptr->vm_end = temp->vm_end;
            vm_ptr->vm_next = temp->vm_next;

            stats->num_vm_area--;
        }
    }
}
void Check_and_merge_on_right(struct vm_area* VMA_head,u64 ret_addr){
    struct vm_area* vm_ptr = VMA_head;
    while(vm_ptr){
        if(vm_ptr->vm_start == ret_addr){
            break;
        }

        vm_ptr = vm_ptr->vm_next;
    }
    if(vm_ptr==VMA_head){
        printk("BC, fat gayi iski toh!\n");
    }
    if(vm_ptr->vm_next==NULL) return;
    else{
        if(vm_ptr->access_flags == vm_ptr->vm_next->access_flags && vm_ptr->vm_end==vm_ptr->vm_next->vm_start){
            vm_ptr->vm_end = vm_ptr->vm_next->vm_end;
            vm_ptr->vm_next = vm_ptr->vm_next->vm_next;

            stats->num_vm_area;
        }
    }
}
/////      End            //////
///////////////////////////////

////// MUNMAP Helper functions //////
/////         Start           //////
struct vm_area* get_the_left_end(struct vm_area* VMA_head, u64 addr){
    struct vm_area* vm_ptr = VMA_head;
    while(vm_ptr && vm_ptr->vm_next){
        if(vm_ptr->vm_end <= addr && addr <= vm_ptr->vm_next->vm_start){
            //vm_ptr it is
            break;
        }
        if(vm_ptr->vm_start < addr && addr < vm_ptr->vm_end){
            vm_ptr->vm_end = addr;
            break;
        }

        vm_ptr = vm_ptr->vm_next;
    }
    //if the addr is in the last node
    if(vm_ptr->vm_next==NULL && vm_ptr->vm_start < addr && addr < vm_ptr->vm_end){
        vm_ptr->vm_end = addr;
    }

    // if(vm_ptr==VMA_head && (vm_ptr->vm_end - vm_ptr->vm_start != 4*KB)){
    //     printk("Gadbad ho gyi hai yahan par!\n");
    //     return NULL;
    // }

    return  vm_ptr;   
}
struct vm_area* get_the_right_end(struct vm_area* VMA_head, u64 right_addr){
    struct vm_area* vm_right = VMA_head;
    while(vm_right){
        if(vm_right->vm_next && (vm_right->vm_end-1)<= right_addr && right_addr < vm_right->vm_next->vm_start){
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
                    vm_right->vm_start += unmap_size;
                    return vm_right;
                    // break;
                }
                else{
                    vm_right->vm_start += (unmap_size/(4*KB) + 1)*4*KB;
                    return vm_right;
                    // break;
                }
            }
        }

        //else keep iterating
        vm_right = vm_right->vm_next;
    }

    if(vm_right == VMA_head){
        printk("Tyranny has happend!\n");
        return NULL;
    }

    return vm_right;
}
/////          End           //////
//////////////////////////////////

////// Mprotect Helper Function //////
///////      Start             //////

//////        End             //////

///////////////////////////////////////////////
//// mprotect system call implementation. ////
/////////////////////////////////////////////
long vm_area_mprotect(struct exec_context *current, u64 addr, int length, int prot)
{
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
        vm->vm_start = vm->vm_start + 4*KB;
        vm->access_flags = 0x0;
        vm->vm_next = NULL;

        current->vm_area = vm;
    }
    if(length==0){
        return 0; //success already
    }
    long end_addr = addr + length;
    // if((length)%(4*KB)!=0){
    //     end_addr = addr + (length/(4*KB) + 1)*(4*KB);
    // }
    // seems like ye chise tb dekhni hai jab hum node ke andar aa chuke hain

    printk("-->OS_protect: Called!!\n");
    print_list(current->vm_area);
    // return 0;

    struct vm_area* vm_ptr = current->vm_area->vm_next;
    while(vm_ptr){
        //case B
        printk("-->OS_protect: While loop started!\n");

        if(vm_ptr->vm_start<=addr && end_addr<vm_ptr->vm_end)
        {
            printk("-->OS_protect: Case B hai \n");
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
                        vm_ptr->access_flags = prot;
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

                    Check_and_merge_on_left(current->vm_area,vm_new->vm_start);

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

                        Check_and_merge_on_left(current->vm_area, vm_new->vm_start);

                        return 0;
                    }
                    else{ //case2

                        printk("-->OS_protect: Case hai\n");

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

                        return 0;
                    }
                }
            }
            else return 0; //already same protection thaa, so no change!
        }
        printk("-->OS_protect: Line346 reached!!!\n");
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
                    Check_and_merge_on_left(current->vm_area,vm_mid->vm_start);
                    Check_and_merge_on_right(current->vm_area,vm_mid->vm_start);
                }

                vm_mid = vm_mid->vm_next;
            }

            return 0;
        }

        vm_ptr = vm_ptr->vm_next;
        printk("-->OS_protect: vm_ptr changed to next!\n");

        if(vm_ptr == NULL){
            printk("-->OS_protect: ptr NULL ho chuka hai bc!\n");
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
    // protection and flag testing
    if(current == NULL){
        return -1; //error
    }

    if(/*protection*/ !(prot == PROT_READ || prot == PROT_READ|PROT_WRITE) || /*flags*/ !(flags==0 || flags == MAP_FIXED)){                                          
        return -1;
    }

    // if vm_area is null
    if(current->vm_area == NULL){
        struct vm_area* vm  = os_alloc(sizeof(struct vm_area));
        vm->vm_start = MMAP_AREA_START;
        vm->vm_start = vm->vm_start + 4*KB;
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

    // printk("----->OS_map: Check left was called!\n");
    Check_and_merge_on_left(VMA_head,ret_addr);
    // printk("----->OS_map: Check right was called!\n");
    Check_and_merge_on_right(VMA_head,ret_addr);
    // printk("----->OS_map: Check right return ho gya!\n");

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
    //Checking
    if(current == NULL){
        return -1; //error
    }
    // if vm_area is null
    if(current->vm_area == NULL){
        struct vm_area* vm  = os_alloc(sizeof(struct vm_area));
        vm->vm_start = MMAP_AREA_START;
        vm->vm_start = vm->vm_start + 4*KB;
        vm->access_flags = 0x0;
        vm->vm_next = NULL;

        current->vm_area = vm;
    }
    struct vm_area* VMA_head = current->vm_area;

    //check if the space to be unmapped lies inside a single node
    struct vm_area* vm_curr = VMA_head;
    int flag_single_node = 0;
    while(vm_curr){
        if(vm_curr->vm_start<addr && (vm_curr->vm_end-1)>(addr+length)){
            flag_single_node = 1;
            long end_addr = addr + length;
            if((length%(4*KB)!=0)){
                end_addr = addr + (length/(4*KB) + 1)*4*KB;
            }

            if(end_addr==vm_curr->vm_end){
                vm_curr->vm_end = addr;
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
            }

            break;
        }
        vm_curr = vm_curr->vm_next;
    }
    if(!flag_single_node){

        //finding the left end node
        struct vm_area* vm_left = get_the_left_end(VMA_head,addr);
        if(vm_left==NULL){
            printk("OS_UNMAP: Left end finder returned NULL\n");
        }

        //finding the right end node
        //here length is not page aligned coz that's correct :)
        u64 right_addr = addr + length; //excluding this boundary addr value

        struct vm_area* vm_right = get_the_right_end(VMA_head,right_addr);
        if(vm_right==NULL){
            printk("OS_UNMAP: ##Chill ho skta hai ye--Right end finder returned NULL\n");
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

long vm_area_pagefault(struct exec_context *current, u64 addr, int error_code)
{
    return -1;
}

/**
 * cfork system call implemenations
 * The parent returns the pid of child process. The return path of
 * the child process is handled separately through the calls at the 
 * end of this function (e.g., setup_child_context etc.)
 */

long do_cfork(){
    u32 pid;
    struct exec_context *new_ctx = get_new_ctx();
    struct exec_context *ctx = get_current_ctx();
     /* Do not modify above lines
     * 
     * */   
     /*--------------------- Your code [start]---------------*/
     

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
  return -1;
}

