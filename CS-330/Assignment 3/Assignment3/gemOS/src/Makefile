all: gemOS.kernel
SRCS = v2p.c
OBJS = boot.o main.o lib.o idt.o kbd.o shell.o serial.o memory.o context.o entry.o apic.o schedule.o mmap.o fork.o page.o  fs.o file.o pipe.o v2p.o 
UOBJS = user/init.o user/lib.o 
CFLAGS  = -nostdlib -nostdinc -fno-builtin -fno-stack-protector -fpic -m64 -I./include -I../include 
LDFLAGS = -nostdlib -z nodefaultlib -q -melf_x86_64 -Tlink64.ld
ASFLAGS = --64  
AS = as 
USER_CFLAGS  = -nostdlib -nostdinc -fno-builtin -fno-stack-protector -fpic -m64 -I./user/


%o: %c
	gcc -c $(CFLAGS) $< -o $@ 

user/%o:user/%c
	gcc -c $(USER_CFLAGS) $< -o $@ 

gemOS.kernel: $(OBJS) $(UOBJS)
	ld $(LDFLAGS) -o $@ $(OBJS) $(UOBJS)

.Phony: clean
clean:
	rm -f v2p.o; rm -f gemOS.kernel; rm -f user/*.o;
