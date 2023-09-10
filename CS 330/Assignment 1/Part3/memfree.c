#include <stdio.h>

int memfree(){
	unsigned long* ptr = (unsigned long*)pr;
	ptr = ptr-1;
	printf("memfree() called\n");
	// check right and left cont free
	unsigned long* right = Check_right_cont(ptr);
	unsigned long* left = Check_left_cont(ptr);

	printf("--------------> LOOPS ENDED\n");

	printf(" right check at %p\nLeft check at %p\n",right,left);

	if(left && right){
		printf("\n------------\n Case 0 \n----------\n");
		// dono side free mem hain
		*left = *left + (unsigned long)*ptr + *right;
		*(left+1) = *(right+1); //combining all 3 mems

		unsigned long* prev = NULL; //prev free mem
		unsigned long* next = NULL; // next free mem

		if(*(right+1)) next = (unsigned long*)*(right+1);
		if(*(left+2)) prev = (unsigned long*)*(left+2);

		printf(" Next Mem at %p\nPrevs Mem at %p\n",next,prev);

		if(!next && !prev){
			// left is head yehi bhramha hai
			head = left;
		}
		else if(!next){
			// next null hai but not prev
			*(prev+1)=0;
			*(head+2) = (unsigned long)left;
			*(left+2)=0; //prev of left
			*(left+1)=(unsigned long)head; //next of left
			head = left;
		}
		else if(!prev){
			// prev null hai but not next
			// phir wahi mtlb ki left head hai
			*(next+2) = (unsigned long) left;
			head = left;
		}
		else{
			// dono hi side null nhi hai 
			*(prev+1) =(unsigned long) next;
			*(next+2) = (unsigned long) prev;
			*(left+2)=0; //prev of left
			*(head+2) = (unsigned long)left;
			*(left+1)=(unsigned long)head; //next of left
			head = left;
		}
		
	}
	else if(left){
		printf("\n------------\n Case 1 \n----------\n");
		// sirf left mein free mem hain
		*left = *left + *ptr;
		unsigned long* prev = NULL; //prev free mem
		unsigned long* next = NULL; // next free mem

		if(*(left+1)) next = (unsigned long*)*(left+1);
		if(*(left+2)) prev = (unsigned long*)*(left+2);

		printf(" Next Mem at %p\nPrevs Mem at %p\n",next,prev);

		if(!next && !prev){
			// left is head yehi bhramha hai
			head = left;
		}
		else if(!next){
			// next null hai but not prev
			*(prev+1)=0;
			*(left+2)=0; //prev of left
			*(head+2)=(unsigned long)left;
			*(left+1)=(unsigned long)head; //next of left
			head = left;
		}
		else if(!prev){
			// prev null hai but not next
			// phir wahi mtlb ki left head hai
			head = left;
		}
		else{
			// dono hi side null nhi hai 
			*(prev+1) =(unsigned long) next;
			*(next+2) = (unsigned long) prev;
			*(left+2)=0; //prev of left
			*(head+2) = (unsigned long)left;
			*(left+1)=(unsigned long)head; //next of left
			head = left;
		}

	}
	else if(right){
		printf("\n------------\n Case 2 \n----------\n");
		// sirf right mein free mem hain
		*ptr = *ptr + *right;
		*(ptr+1) = *(right+1);
		*(ptr+2) = *(right+2);
		right = ptr;

		unsigned long* prev = NULL; //prev free mem
		unsigned long* next = NULL; // next free mem

		if(*(right+1)) next = (unsigned long*)*(right+1);
		if(*(right+2)) prev = (unsigned long*)*(right+2);

		printf(" Next Mem at %p\nPrevs Mem at %p\n",next,prev);

		if(!next && !prev){
			// right is head yehi bhramha hai
			head = right;
		}
		else if(!next){
			// next null hai but not prev
			*(prev+1)=0;
			*(right+2)=0; //prev of left
			*(head+2) = (unsigned long)right;
			*(right+1)= (unsigned long)head; //next of left
			head = right;
		}
		else if(!prev){
			// prev null hai but not next
			// phir wahi mtlb ki left head hai
			head = right;
		}
		else{
			// dono hi side null nhi hai 
			*(prev+1) =(unsigned long) next;
			*(next+2) = (unsigned long) prev;
			*(right+2)=0; //prev of left
			*(head+2) = (unsigned long)right;
			*(right+1)=(unsigned long)head; //next of left
			head = right;
		}
	}
	else{
		printf("\n------------\n Case 3 \n----------\n");
		// dono hi side free nhi hain
		*(ptr+2) = 0;
		*(head+2) = (unsigned long)ptr;
		*(ptr+1) = (unsigned long) head;
		head = ptr;
	}
	return 0;
}