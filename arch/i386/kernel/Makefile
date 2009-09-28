#Hand written Makefile for the Ufeox kernel

VPATH = ../tests:../include:../include/tests:../include/ufeox

LINKER_SCRIPT = Ufeox.ld

SRCS =  common.c monitor.c vga.c vsprintf.c clib.c gdt.c idt.c isr.c apic.c timer.c paging.c
TESTSRCS = tests.c t_sprintf.c

HEADERS = common.h monitor.h vga.h kernel.h clib.h gdt.h idt.h isr.h apic.h timer.h paging.h
TESTHEADERS = tests.h t_sprintf.h

ASMSRCS = boot.S kernel.S vectors.S


OBJS = $(patsubst %.c,%.o,$(SRCS))
ASMOBJS = $(patsubst %.S,%.o,$(ASMSRCS))
TESTOBJS = $(patsubst %.c,%.o,$(TESTSRCS))

#$(error $(OBJS))

BIN = Ufeox-0.0.1.img
CC = gcc
CPPFLAGS = -Wall -I../include -fno-stack-protector -ffreestanding -O0	
#ASFLAGS = -felf


$(BIN): depend $(OBJS)  $(ASMOBJS) $(TESTOBJS) 
	@printf "\n\nBuilding final executable. Date: "
	@date 
	@printf "\n"
	ld -o $(BIN) $(OBJS) $(ASMOBJS) $(TESTOBJS) -T $(LINKER_SCRIPT)



depend: $(HEADERS) Makefile Ufeox.ld $(TESTHEADERS)
	@printf "\nNOTE: Some files have changed. Cleaning up for complete rebuild.\n\n"
	@rm -f *.o $(BIN) 
	@touch depend


clean:
	rm -vf *.o $(BIN) 
