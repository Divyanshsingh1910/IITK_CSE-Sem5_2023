#include<context.h>
#include<memory.h>
#include<lib.h>
#include<entry.h>
#include<file.h>
#include<tracer.h>

// Declaring the modes
#define   O_READ 0x1
#define   O_WRITE 0x2
#define   O_RDWR (O_READ|O_WRITE)

#define Delimiter 111000111

// Max number of files
#define MAX_OPEN_FILES 16

// // file types
// enum{
// 	STDIN,
// 	STDOUT,
// 	STDERR,
// 	REGULAR,
// 	TRACE_BUFFER,
// 	MAX_FILE_TYPE,
// };

int get_args(u64 syscall_num){

	int n_args[100];
	n_args[1] = 1; // exit
	n_args[2] = 0; // getpid
	n_args[4] = 2; // expand
	// n_args[5] = ; // shrink
	// n_args[6] = ; // alarm
	n_args[7] = 1; // sleep
	n_args[8] = 2; // signal
	n_args[9] = 2; // clone
	n_args[10] = 0; // fork
	n_args[11] = 0; // stats
	n_args[12] = 1; // configure
	n_args[13] = 0; // phys_info
	n_args[14] = 1; // dump_ptt
	n_args[15] = 0; // cfork
	n_args[16] = 4; // mmap
	n_args[17] = 2; // munmap
	n_args[18] = 3; // mprotect
	n_args[19] = 1; // pmap
	n_args[20] = 0; // vfork
	n_args[21] = 0; // get_user_p
	n_args[22] = 0; // get_cow_f
	n_args[23] = 2; // open
	n_args[24] = 3; // read
	n_args[25] = 3; // write
	n_args[27] = 1; // dup
	n_args[28] = 2; // dup2
	n_args[29] = 1; // close
	n_args[30] = 3; // lseek
	n_args[35] = 4; // ftrace
	n_args[36] = 1; // trace_buffer
	n_args[37] = 2; // start_strace
	n_args[38] = 0; // end_strace
	n_args[39] = 3; // read_strace
	n_args[40] = 2; // strace
	n_args[41] = 3; // read_ftrace
	// n_args[61] = ; // getppid

	return n_args[syscall_num];

}

///////////////////////////////////////////////////////////////////////////
//// 		        Start of Trace buffer functionality 		      /////
///////////////////////////////////////////////////////////////////////////

int is_valid_mem_range(unsigned long buff, u32 count, int access_bit) 
{	
	// 1.2 part of the assignment
	// iterate through the mms segments 
	//iterate through all the vm-areas
	printk("OS: address of buff passed: %d\n",buff);

	struct exec_context* current_pcb = get_current_ctx();
	struct mm_segment* curr_mms = current_pcb->mms;

	// if buff in mms segment

	if(curr_mms[MM_SEG_CODE].start <= buff && buff < curr_mms[MM_SEG_STACK].end){
		// printk("OS: Found in mms \n");
		// iterate through mms segments
		// Code segment
		if(curr_mms[MM_SEG_CODE].start <= buff && buff < curr_mms[MM_SEG_CODE].next_free){
			printk("OS: Found in mms CODE \n");
			if(access_bit==0) return 0;
		}

		// ROData segment
		if(curr_mms[MM_SEG_RODATA].start <= buff && buff < curr_mms[MM_SEG_RODATA].next_free){
			printk("OS: Found in mms RODATA \n");
			if(access_bit==0) return 0;
		}

		// for any other segment
		return 1;
	}

	// buff in any vm_area
	struct vm_area* curr_vm = current_pcb->vm_area;

	// iterate through all the vms
	while(curr_vm!=NULL){
		if(curr_vm->vm_start <= buff && buff <= curr_vm->vm_end){
			printk("OS: Found the vm with access: %d\n",curr_vm->access_flags);

			if(access_bit == curr_vm->access_flags-1) return 0;
		}

		curr_vm = curr_vm->vm_next;
	}

	return 1;
}



long trace_buffer_close(struct file *filep)
{
	// 1.1(d) part of the assignment

	struct exec_context* current_ctx_pcb = get_current_ctx();

	int fd = filep->trace_buffer->fd_of_trace_buffer;
	
	os_page_free(USER_REG, filep->trace_buffer->head_of_trace_buffer); //4KB mem

	os_free(filep->trace_buffer, sizeof(struct trace_buffer_info)); // buffer info free
	os_free(filep->fops,sizeof(struct fileops));//file ops free

	os_free(filep,sizeof(struct file)); //file pointer free

	// pointing the array of files[fd] to null
	current_ctx_pcb->files[fd] = NULL;

	return 0;	
}



int trace_buffer_read(struct file *filep, char *buff, u32 count)
{
    // 1.1(c) part of the assignment
	if(filep->mode == O_WRITE) return -EINVAL;

	////////// Performing mem_check //////////
	/*
	unsigned long buff_addr = (unsigned long) &buff[0];
	if(!is_valid_mem_range(buff_addr, count, 0)) return -EBADMEM;

	printk("Running trace_buffer_read atm\n");
	*/

	// if write_off > read_off --> read upto to write_off else wrap around

	struct trace_buffer_info* trace_p = filep->trace_buffer;
	int write_offset = trace_p->write_offs_of_trace_buffer;
	int read_offset = trace_p->read_offs_of_trace_buffer;
	char *head_p = trace_p->head_of_trace_buffer;

	if(trace_p->read_left==0) return 0; //Nothing to read

	//if reading to many bytes
	// count = count%trace_p->read_left;

	int write_flag = 0; //it becomes 1 when it encounters write_offset

	int count_read = 0;

	int index = read_offset;
	
	if(write_offset==0 && read_offset==0){
		//initially pura likh diya
		buff[0] = head_p[index];
		index++;
		count_read++;
	}

	while( index < 4096 && index-read_offset<count){
		if(index==write_offset){
			write_flag = 1; //write_offset encountered here, won't write here
			break;
		}
		//else simply read
		buff[index-read_offset] = head_p[index];

		count_read++;
		index++;
	}
	// update the read offset
	trace_p->read_offs_of_trace_buffer = index; //read till (index-1)th byte

	if(index!= read_offset + count_read){
		printk("OS_READ: Kuch to gadbad hai daya!\n");
	}

	int checkpoint = count_read;

	if(!write_flag && count_read<count){
		// didn't encounter write_flag --> wrap around

		index = 0;
		while(index!=write_offset && count_read<count){
			buff[checkpoint+index] = head_p[index];

			count_read++;
			index++;
		}

		// wrap around update;
		trace_p->read_offs_of_trace_buffer = index; //read upto (index-1)th byte
	}
	// if read all the bytes till last
	if(trace_p->read_offs_of_trace_buffer == 4096){
		printk("OS_READ_DEBUG: This happended!\n");
		trace_p->read_offs_of_trace_buffer = 0;
	}

	printk("OS_READ: New read offset: %d\n",trace_p->read_offs_of_trace_buffer);
	printk("OS_READ: Bytes read: %d\n",count_read);

	//updating these counts
	trace_p->read_left-= count_read;
	trace_p->write_left = 4096 - trace_p->read_left;


	return count_read;

	return 0;
}


int trace_buffer_write(struct file *filep, char *buff, u32 count)
{	
	// 1.1(b) part of the assignment
	if(filep->mode == O_READ) return -EINVAL;

	printk("trace_buffer_write function running atm\n");

	////////// Performing mem_check //////////
	/*
	unsigned long buff_addr = (unsigned long) &buff[0];
	int k = is_valid_mem_range((unsigned long) buff, count, 1);

	if(!k) return -EBADMEM;
	
	printk("Check returned: %d\n",k);
	*/

	// if read_off > write_off --> write upto read_off
	// else wrap around and read upto read_off

	struct trace_buffer_info* trace_p = filep->trace_buffer;
	int write_offset = trace_p->write_offs_of_trace_buffer;
	int read_offset = trace_p->read_offs_of_trace_buffer;
	char *head_p = trace_p->head_of_trace_buffer;

	// printk("\n-----\nPrinting the status of the trace buffer here: \n");
	// printk("Fd of the buffer: %d\n",trace_p->fd_of_trace_buffer);
	// printk("Write offset of the trace buffer: %d\n",write_offset);
	// printk("Read offset of the trace buffer: %d\n",read_offset);
	// printk("---------\n");

	if(trace_p->write_left==0){
		printk("OS_TRACE_BF_WRITE: Exited from here\n");
		return 0; //No space left to write anything else
	}

	//if writing to many bytes
	// count = count%trace_p->write_left;

	int read_flag = 0; //it becomes 1 when it encounter read_offset

	int count_written = 0;

	int index = write_offset; //starting writing from this index (inclusive)

	head_p[index] = buff[0]; // 1 byte written ( to change from R==W {for first write} )
	index++;
	count_written++;

	while( index < 4096 && (index-write_offset) < count){
		if(index == read_offset){
			read_flag = 1; //read_offset encountered here, won't write here
			break;
		}
		//else just the write the content of buff to trace
		head_p[index] = buff[index-write_offset];

		count_written++;
		index++;
	}

	// update the write offset
	trace_p->write_offs_of_trace_buffer = index; //written upto (index-1)th byte 

	if(index!= write_offset + count_written){
		printk("OS_WRITE: kuch to gadbad hai daya!\n");
	}

	int checkpoint = index;

	if(!read_flag && count_written<count){
		// didn't encounter the read_offset --> wrap around
		index = 0;
		while(index!=read_offset && count_written<count){
			head_p[index] = buff[checkpoint - write_offset + index];

			count_written++;
			index++;
		}

		// wrap around offset update
		trace_p->write_offs_of_trace_buffer = index; //written upto (index-1)th byte
	}

	// if wrote complete space then start writing from 0 once read;
	if(trace_p->write_offs_of_trace_buffer==4096) trace_p->write_offs_of_trace_buffer = 0;

	printk("OS_WRITE: New write offset is: %d\n",trace_p->write_offs_of_trace_buffer);
	printk("OS_WRITE: Bytes written: %d\n",count_written);

	// if(count == 4){
	// 	printk("\n----\n");
	// 	printk("x char: %d\n",head_p[4090]);
	// 	printk("-0th char: %d\n",head_p[4095]);
	// 	printk("-1st char: %d\n",head_p[4094]);
	// 	printk("0th char: %d\n",head_p[0]);
	// 	printk("1st char: %d\n",head_p[1]);
	// 	printk("\n----\n");
	// }

	trace_p->write_left-= count_written; //updating the write space left
	trace_p->read_left = 4096 - trace_p->write_left; //updating the read space as well

	printk("-------OS_WRITE: Returning from trace write function\n");
	return count_written;

    return 0;
}

int sys_create_trace_buffer(struct exec_context *current, int mode)
{
	// 1.1(a) part of the assignment 
	int fd = -1; 
	struct file* file_p = os_alloc(sizeof(struct file));

	// find the lowest unused fd
	struct file** all_files = current->files;

	for(int i=0; i<MAX_OPEN_FILES; i++){
		if(all_files[i]==NULL){
			fd = i;
			all_files[i] = file_p;
			break;
		}
	}
	if(fd==-1) return -EINVAL;
	/*
	 a free file pointer file_p with fd  
	*/
    
	file_p->type = TRACE_BUFFER;
	file_p->mode = mode;
	file_p->offp = 0;
	file_p->inode = NULL;
	
	// allocate trace buffer
	struct trace_buffer_info* trace_p = os_alloc(sizeof(struct trace_buffer_info));

	trace_p->fd_of_trace_buffer = fd;
	trace_p->head_of_trace_buffer = (char*) os_page_alloc(USER_REG);
	trace_p->write_offs_of_trace_buffer = 0;
	trace_p->read_offs_of_trace_buffer = 0;
	trace_p->read_left = 0;
	trace_p->write_left = 4096;

	file_p->trace_buffer = trace_p;

	// fops allocation
	struct fileops* fops_ = os_alloc(sizeof(struct fileops));

	fops_->read = trace_buffer_read;
	fops_->write = trace_buffer_write;
	fops_->close = trace_buffer_close;

	file_p->fops = fops_;

	// return fd
	printk("fd of created trace buffer is: %d\n",fd);

	return fd; 
	// return 0;
}

///////////////////////////////////////////////////////////////////////////
//// 	        	Start of strace functionality 		      	      /////
///////////////////////////////////////////////////////////////////////////


int perform_tracing(u64 syscall_num, u64 param1, u64 param2, u64 param3, u64 param4)
{
	// 2.3 part of the assignment
	// printk("Printing all the parameters: \n");
	// printk("syscall_num: %d param2: %x &param2: %x\n",syscall_num,param2,&param2);
	// printk("syscall_num: %d param3: %x &param3: %x\n",syscall_num,param3,&param3);
	// printk("syscall_num: %d param4: %x &param4: %x\n",syscall_num,param4,&param4);
	

	struct exec_context* current_pcb = get_current_ctx();
	struct strace_head* base_head = current_pcb->st_md_base;

	if(base_head==NULL){
		base_head = os_alloc(sizeof(struct strace_head));
		base_head->count = 0;
		base_head->strace_fd = 0;
		base_head->is_traced = 0;
		base_head->next = NULL;
		base_head->last = NULL;

		current_pcb->st_md_base = base_head;
		return 0;
	}

	if(base_head->is_traced==0 || syscall_num == 38){
		//tracing is off || end_strace called
		return 0;
	}

	//check syscall if filtered_mode on
	if(base_head->tracing_mode == FILTERED_TRACING){
		int flag = 0;
		struct strace_info* info_ptr = base_head->next;
		while(info_ptr!=NULL){
			if(info_ptr->syscall_num==syscall_num){
				flag = 1;
				break;
			}
			info_ptr = info_ptr->next;
		}

		if(flag==0) return 0;
	}
	printk("\n-----\nWorking for sycall: %d\n-----\n",syscall_num);
	// get the strace_buffer first
	int fd = base_head->strace_fd;

	struct trace_buffer_info* trace_p = current_pcb->files[fd]->trace_buffer;
	struct file* filep = current_pcb->files[fd];
	//got the trace_p and filep

	u64 end = -1; //end of a syscall

	int n_args = get_args(syscall_num);
	int ret = trace_buffer_write(filep,&syscall_num,8);
	if(ret!=8) printk("OS_PT: Couldn't write syscall\n");

	// if(n_args==0){
	// 	// tackle
	// }
	if(n_args==1){
		// code
		ret = trace_buffer_write(filep,&param1,8);
		if(ret!=8) printk("OS_PT: Couldn't write meta param1\n");
	}

	if(n_args==2){
		//code
		ret = trace_buffer_write(filep,&param1,8);
		if(ret!=8) printk("OS_PT: Couldn't write meta param1\n");

		ret = trace_buffer_write(filep,&param2,8);
		if(ret!=8) printk("OS_PT: Couldn't write meta param2\n");
	}
	if(n_args==3){
		//code
		ret = trace_buffer_write(filep,&param1,8);
		if(ret!=8) printk("OS_PT: Couldn't write meta param1\n");

		ret = trace_buffer_write(filep,&param2,8);
		if(ret!=8) printk("OS_PT: Couldn't write meta param2\n");

		ret = trace_buffer_write(filep,&param3,8);
		if(ret!=8) printk("OS_PT: Couldn't write meta param3\n");
	}
	if(n_args==4){
		//code
		ret = trace_buffer_write(filep,&param1,8);
		if(ret!=8) printk("OS_PT: Couldn't write meta param1\n");

		ret = trace_buffer_write(filep,&param2,8);
		if(ret!=8) printk("OS_PT: Couldn't write meta param2\n");

		ret = trace_buffer_write(filep,&param3,8);
		if(ret!=8) printk("OS_PT: Couldn't write meta param3\n");

		ret = trace_buffer_write(filep,&param4,8);
		if(ret!=8) printk("OS_PT: Couldn't write meta param4\n");
	}

	u64 meta_data = 0;
	ret = trace_buffer_write(filep,&meta_data,8);
	if(ret!=8) printk("OS_PT: Couldn't write meta data\n");

	return 0;

}


int sys_strace(struct exec_context *current, int syscall_num, int action)
{
	// 2.4 part of the assignment 
	struct strace_head* base_head = current->st_md_base;
	if(action == ADD_STRACE){
		//need to add the syscall for tracing
		struct strace_info* info_ptr = base_head->last;

		struct strace_info* new_info = os_alloc(sizeof(struct strace_info));
		new_info->syscall_num = syscall_num;
		new_info->next = NULL;

		if(!info_ptr){
			//if it is NULL

			base_head->next = new_info;
			base_head->last = new_info;
		}
		else{
			base_head->last->next = new_info;
			base_head->last = new_info;
		}
	}
	else{
		// REMOVE_STRACE
		//if only one node and that is yehi
		if(base_head->next == base_head->last && base_head->next->syscall_num==syscall_num){
			os_free(base_head->next,sizeof(struct strace_info));
			base_head->next = NULL;
			base_head->last = NULL;
		}

		struct strace_info* info_ptr = base_head->next;
		while(info_ptr && info_ptr!= base_head->last){
			if(info_ptr->next->syscall_num == syscall_num){
				struct strace_info* temp = info_ptr->next;
				info_ptr->next == info_ptr->next->next;
				os_free(temp,sizeof(struct strace_info));
				break;
			}
		}
		// last mein tha if 
		if(info_ptr->next==NULL){
			base_head->last = info_ptr; // make this last
		}

		// first node ko check hi nhi kiya
		if(base_head->next->syscall_num == syscall_num){
			struct strace_info* temp = base_head->next;
			base_head->next = base_head->next->next;
			os_free(temp,sizeof(struct strace_info));
		}
	}

	return 0;
}

int sys_read_strace(struct file *filep, char *buff, u64 count)
{
	// 2.5 part of the assignment
	if(count==0) return 0;
	int write_offs = 0;
	int ret, n_args;
	// char read8[8]; // the reading which will be done
	u64 syscall_num;

	while(count>0){
		ret = trace_buffer_read(filep,&syscall_num,8);
		if(ret==0){
			printk("OS_RS: Nothing left strace_buffer, returning\n");
			return write_offs;
		}
		if(ret!=8){
			printk("OS_RS: couldn't read syscall num\n");
		}
		n_args = get_args(syscall_num);

		for (int i = 0; i < 8; i++) {
			char byte = (syscall_num >> (i*8)) & 0xFF;
			buff[write_offs+i] = byte;
		}
		write_offs += 8;

		
		ret = trace_buffer_read(filep,buff+write_offs,n_args*8+8); //meta also read

		if(ret!= n_args*8+8){
			printk("OS_RS: couldn't read args\n");
		}

		write_offs+= 8*n_args; //metadata will be overwritten

		count--;
	}
	return write_offs;
	return 0;
}

int sys_start_strace(struct exec_context *current, int fd, int tracing_mode)
{
	// 2.1 part of the assignment
	struct strace_head* base_head = current->st_md_base;
	base_head->count = 0;
	base_head->is_traced = 1;
	base_head->strace_fd = fd;
	base_head->tracing_mode = tracing_mode;

	return 0;
}

int sys_end_strace(struct exec_context *current)
{
	// 2.2 part of the assignment
	struct strace_head* base_head = current->st_md_base;
	base_head->is_traced = 0;

	struct strace_info* info_ptr = base_head->next;
	struct strace_info* temp_ptr;
	while(info_ptr!=base_head->last){
		temp_ptr = info_ptr->next;

		os_free(info_ptr,sizeof(struct strace_info));

		info_ptr = temp_ptr;
	}

	os_free(base_head->last,sizeof(struct strace_info));
	
	return 0;
}



///////////////////////////////////////////////////////////////////////////
//// 		      Start of ftrace functionality 		      	      /////
///////////////////////////////////////////////////////////////////////////

void check_and_disable_function(struct exec_context *ctx,struct ftrace_info* fn_info){
	//check if tracing enabled?
	//call the disable function if enabled
	// checking code: if the 1st 32 bits are 0xFF or not

	u64* Faddr64 = (u64 *)fn_info->faddr;
	u64 orgval64 = *(Faddr64); //deferencing

	u32 orgval32 = (u32)orgval64; //taking the first 32 bits	

	if(orgval32 == 0xFF){
		do_ftrace(ctx,fn_info->faddr,fn_info->num_args,DISABLE_FTRACE, fn_info->fd);
	}
}

void check_and_enable_function(struct exec_context *ctx,struct ftrace_info* fn_info){
	//check if tracing disabled?
	//call the enable function if disabled
	// checking code: if the 1st 32 bits are 0xFF or not

	u64* Faddr64 = (u64 *)fn_info->faddr;
	u64 orgval64 = *(Faddr64); //deferencing

	u32 orgval32 = (u32)orgval64; //taking the first 32 bits	

	if(orgval32 != 0xFF){
		printk("-------OS_check-n-enable: Not enabled-->calling enable for faddr: %d\n",fn_info->faddr);
		long ret = do_ftrace(ctx,fn_info->faddr,ENABLE_FTRACE,0, fn_info->fd);
	}
	else{
		printk("-------OS_check-n-enable: Already enabled for faddr: %d\n",fn_info->faddr);
	}
}

long do_ftrace(struct exec_context *ctx, unsigned long faddr, long action, long nargs, int fd_trace_buffer)
{
	// printk("OS_DO_ftrace: exercised\n");
	if(ctx->ft_md_base == NULL){
		//allocate base
		printk("OS_ftrace_ENABLE: List was Null, initialized it\n");
		struct ftrace_head* ft_base = os_alloc(sizeof(struct ftrace_head));

		ft_base->count=0;
		ft_base->next = NULL;
		ft_base->last = NULL;

		ctx->ft_md_base = ft_base;
	}

	if(action == ADD_FTRACE){
		// add the function to the list

		printk("OS_DO_ftrace: ADD_FTRACE was exercised\n");

		struct ftrace_head* ft_base = ctx->ft_md_base;
		if(ft_base->count == FTRACE_MAX){
			// already enough functions in the list
			return -EINVAL;
		}

		struct ftrace_info* info_ptr = ft_base->next;

		if(info_ptr==NULL){
			printk("OS_ftrace_ENABLE: List abhi NULL hai\n");
		}

		while(info_ptr){
			if(info_ptr->faddr == faddr){
				//function already in the list
				printk("OS_ftrace_ENABLE: function already exists!!\n");
				return -EINVAL;
			}

			info_ptr = info_ptr->next; //updating
		}

		// no errors just store the function in the list
		struct ftrace_info* new_info = os_alloc(sizeof(struct ftrace_info));
		new_info->faddr = faddr;
		new_info->num_args = nargs;
		new_info->fd = fd_trace_buffer;
		new_info->next = NULL;

		if(ft_base->last==NULL){
			//NULL hai list
			ft_base->last = new_info;
			ft_base->next = new_info;
		}
		else{
			//List is not NULL
			ft_base->last->next = new_info;
			ft_base->last = new_info;
		}

		ft_base->count ++;

		printk("OS_DO_ftrace: New last is: %d\n",ft_base->last->faddr);
		printk("--------OS_ftrace_ADD: Adding done for faddr: %d\n",faddr);
		return 0;
	}

	if(action == REMOVE_FTRACE){

		struct ftrace_head* ft_base = ctx->ft_md_base;

		struct ftrace_info* info_ptr = ft_base->next;
		int found_flag = 0; //1 when faddr of func_to_remove is found

		while(info_ptr->next!= ft_base->last){
			if(info_ptr->next->faddr == faddr){
				found_flag = 1;

				struct ftrace_info* temp = info_ptr->next;
				//add and check_and_disable_function
				check_and_disable_function(ctx,temp);

				info_ptr->next = info_ptr->next->next;

				os_free(temp,sizeof(struct ftrace_info));
			}
			info_ptr = info_ptr->next; //updating
		}

		if(ft_base->last->faddr==faddr){
			found_flag = 1;

			os_free(ft_base->last,sizeof(struct ftrace_info));

			ft_base->last = info_ptr;
		}

		if(ft_base->next->faddr == faddr){
			found_flag = 1;
			info_ptr = ft_base->next;

			os_free(ft_base->next,sizeof(struct ftrace_info));

			ft_base->next = info_ptr;
		}
		//failure
		if(found_flag==0){
			return -EINVAL;
		}
		//success
		ft_base->count--;
		return 0;
	}

	if(action == ENABLE_FTRACE){

		// generate the fault --> manipulate the func addr
		struct ftrace_head* ft_base = ctx->ft_md_base;
		struct ftrace_info* fn_info = ft_base->next;

		printk("------OS_ENable_ft: called for %d\n",faddr);

		int found_flag = 0;
		while(fn_info){
			if(fn_info->faddr == faddr){
				found_flag = 1;
				break;
			}

			fn_info = fn_info->next;
		}

		if(!found_flag){
			printk("OS_ftrace_ENABLE: faddr not found\n");
			if(ft_base->next==NULL && ft_base->next == ft_base->last){
				printk("OS_ftrace_ENABLE: list empty thi isliye nahi mila\n");
			}
			return - EINVAL;
		} //function not found;

		//Code to manipulate the address space starts
		//The code below is not verfied......

		u64* Faddr64 = (u64 *)fn_info->faddr;
		u64 orgval64 = *(Faddr64); //deferencing
		
		// printk("OS_ftrace_ENABLE: Previous orgval64: %x\n",orgval64);

		u8* prev_ins = Faddr64;
		fn_info->code_backup[0] = *(prev_ins);
		*prev_ins = 0xFF;
		fn_info->code_backup[1] = *(prev_ins + 1);
		*(prev_ins+1) = 0xFF;
		fn_info->code_backup[2] = *(prev_ins + 2);
		*(prev_ins+2) = 0xFF;
		fn_info->code_backup[3] = *(prev_ins + 3);
		*(prev_ins+3) = 0xFF;

		orgval64 = *(Faddr64);

		printk("--------OS_ftrace_ENABLE: Enabling ftrace done for faddr: %d\n",faddr);

		return 0;

	}

	if(action == DISABLE_FTRACE){
		// Undo the fault --> manipulate the func addr
		struct ftrace_head* ft_base = ctx->ft_md_base;
		struct ftrace_info* fn_info = ft_base->next;

		int found_flag = 0;
		while(fn_info){
			if(fn_info->faddr == faddr){
				found_flag = 1;
				break;
			}

			fn_info = fn_info->next;
		}

		if(!found_flag){
			printk("OS_ftrace_DISABLE: faddr not found\n");
			return - EINVAL;
		} //function not found;

		//Code to restore the original instruction
		//Note: Code not verifed or human evaluated

		u64* Faddr64 = (u64 *)fn_info->faddr;
		u64 orgval64 = *(Faddr64); //deferencing
		
		printk("OS_ftrace_ENABLE: Previous orgval64: %x\n",orgval64);

		u8* prev_ins = Faddr64;
		*(prev_ins) = fn_info->code_backup[0];

		*(prev_ins + 1) = fn_info->code_backup[1];

		*(prev_ins + 2) = fn_info->code_backup[2];
	
		*(prev_ins + 3) = fn_info->code_backup[3];

		// u32 orgval32 = (u32)orgval64; //taking the first 32 bits

		// fn_info->code_backup[0] = (orgval32 >> 0) & 0xFF;
		// fn_info->code_backup[1] = (orgval32 >> 8) & 0xFF;
		// fn_info->code_backup[2] = (orgval32 >> 16) & 0xFF;
		// fn_info->code_backup[3] = (orgval32 >> 24) & 0xFF;

		// orgval32 = 0xFF;

		// orgval64 = (orgval64 & 0xFFFFFFFF00000000) | orgval32;

		orgval64 = *(Faddr64);

		printk("OS_ftrace_ENABLE: New orgval64: %x\n",orgval64);

		return 0;

	}

	if(action == ENABLE_BACKTRACE){
		// enable call-back trace
		struct ftrace_head* ft_base = ctx->ft_md_base;
		struct ftrace_info* fn_info = ft_base->next;

		int found_flag=0;
		while(fn_info){
			if(fn_info->faddr == faddr){
				found_flag = 1;
				fn_info->capture_backtrace = 1;
				check_and_enable_function(ctx,fn_info);
				break;
			}

			fn_info = fn_info->next;
		}

		// fail
		if(found_flag==0){
			printk("OS_ENABLE_BACKT: faddr not found in the trace list\n");
			return -EINVAL;
		}
		printk("--------OS_ftrace_ENABLE_BACK: Enabling done for faddr: %d\n",faddr);
		//success
		return 0;
	}

	if(action == DISABLE_BACKTRACE){
		// do this
		struct ftrace_head* ft_base = ctx->ft_md_base;
		struct ftrace_info* fn_info = ft_base->next;

		int found_flag=0;
		while(fn_info){
			if(fn_info->faddr == faddr){
				found_flag = 1;
				fn_info->capture_backtrace = 0;
				check_and_disable_function(ctx,fn_info);
				break;
			}

			fn_info = fn_info->next;
		}

		// fail
		if(found_flag==0){
			printk("OS_DISABLE_BACKT: faddr not found in the trace list\n");
			return -EINVAL;
		}

		//success
		return 0;
	}
    return 0;
}

// writing the arguments into buffer
void write_args(struct user_regs* regs,u32 num_args,struct file* filep){
	/*
	...
	code to write args in buffer
	...
	*/
	int ret;
	if(num_args==0) return;

	ret = trace_buffer_write(filep,&(regs->rdi),8);
	if(num_args==1) return;

	ret = trace_buffer_write(filep,&(regs->rsi),8);
	if(num_args==2) return;

	ret = trace_buffer_write(filep,&(regs->rdx),8);
	if(num_args==3) return;

	ret = trace_buffer_write(filep,&(regs->rcx),8);
	if(num_args==4) return;

	ret = trace_buffer_write(filep,&(regs->r8),8);
	if(num_args==5) return;

	return;
}

//Fault handler
long handle_ftrace_fault(struct user_regs *regs)
{
	printk("---------OS_HANDLE_ft_fault: This call was exercised for faddr: %d\n",regs->entry_rip);

	u64 faddr = (u64) regs->entry_rip;
	struct exec_context* ctx = get_current_ctx();
	struct ftrace_head* ft_base = ctx->ft_md_base;
	struct ftrace_info* fn_info = ft_base->next;

	// printk("OS_HANDLE_ft_fault:Printing the function list: \n");
	// while(fn_info){
	// 	printk("OS_HANDLE_ft_fault: faddr of this fn: %d\n",fn_info->faddr);
	// 	fn_info = fn_info->next;
	// }
	// printk("OS_HANDLE_ft_fault:Ended\n");

	// printk("OS_HANDLE_ft_fault: Going to search for faddr: %d\n",faddr);
	int found_flag = 0;
	fn_info = ft_base->next;
	while(fn_info){
		if(fn_info->faddr == faddr){
			found_flag = 1;
			break;
		}
		// printk("OS_HANDLE_ft_fault: Matching with faddr: %d\n",fn_info->faddr);
		fn_info = fn_info->next;
	}

	if(found_flag==0){
		printk("OS_HANDLE_ft_fault: faddr milaa hi nhi\n");
		return -EINVAL;
	}
	printk("OS_HANDLE_ft_fault: Mil gya faddr bc!\n");

	struct file* filep = ctx->files[fn_info->fd];
	struct trace_buffer_info* trace_p = filep->trace_buffer;

	/*
	...
	handling the correct functionality of the function
	...
	*/

	regs->entry_rsp -= 8;
	*((u64 *) regs->entry_rsp) = regs->rbp;
	regs->rbp = regs->entry_rsp;
	regs->entry_rip += 8;

	int ret;
	//writing func addr
	ret = trace_buffer_write(filep,&faddr,8);

	if(ret!=8) printk("OS_HANDLE_ft_fault: Couldn't write the function, ret = %d\n",ret);

	//writing args
	write_args(regs,fn_info->num_args,filep);

	printk("\n--------OS_HANDLE_ft_fault: Arguments writing done for faddr: %d\n\n",faddr);


	if(fn_info->capture_backtrace == 1){
		//if backtrace

		printk("------OS_HANDLE_ft_fault: back tracing started for faddr: %d\n",faddr);
		// addr to 1st instruction of the func
		ret = trace_buffer_write(filep,&faddr,8);

		u64* ptr = regs->rbp; 
		u64 ret_addr = *(ptr+1); //u64 ret_addr = *((u64)rbp + 8);
		while(ret_addr != END_ADDR){
			printk("\n------OS_HANDLE_ft_fault: New ret_addr: %x\n\n",ret_addr);
			ret = trace_buffer_write(filep,&ret_addr,8);
			
			ptr = *ptr; //rbp = (rbp)[0];

			printk("\n------OS_HANDLE_ft_fault: rbp updated, new rbp = %x\n\n",ptr);

			ret_addr = *(ptr+1); //ret_addr = *((u64)rbp + 8);

			printk("#######OS_HANDLE_ft_fault: While loop samapt huaa!!!\n");
		}
	}

	/*
	...
	code to add meta data
	...
	*/
	u64 meta_data = Delimiter;
	ret = trace_buffer_write(filep,&meta_data,8);

	if(ret!=8) printk("OS_HANDLE_ft_fault: Couldn't write the metadata, ret = %d\n",ret);


	printk("OS_HANDLE_ft_fault: Exiting the fault handler\n");

    return 0;
}


int sys_read_ftrace(struct file *filep, char *buff, u64 count)
{
	if(count == 0) return 0;

	int write_offs = 0;
	int ret, n_args;

	u64 data;

	while(count>0){
		ret = trace_buffer_read(filep,&data,8);
		if(ret==0){
			printk("OS_RFT: Nothing left in ftrace buffer, returning\n");
			return write_offs;
		}
		if(ret!=8){
			printk("OS_RS: couldn't read ftrace buffer\n");
		}

		while(data!=Delimiter){
			//write in buff
			for (int i = 0; i < 8; i++) {
				char byte = (data >> (i*8)) & 0xFF;
				buff[write_offs+i] = byte;
			}
			write_offs += 8;

			ret = trace_buffer_read(filep,&data,8);

			if(ret==0){
				printk("OS_RFT: Nothing left in ftrace buffer, returning\n");
				return write_offs;
			}
			if(ret!=8){
				printk("OS_RS: couldn't read ftrace buffer\n");
			}

		}

		count--;
	}	

	return write_offs;
    return 0;
}



