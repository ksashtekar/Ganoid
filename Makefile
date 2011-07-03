#Makefile for the Ganoid kernel

export

OUTDIR        := bin

VERSION       := 0.0.1	
ARCH          := i386
LINKER_SCRIPT := arch/$(ARCH)/kernel/ganoid.ld

VPATH         := arch/$(ARCH)/drivers:arch/$(ARCH)/boot: \
                 arch/$(ARCH)/lib:arch/$(ARCH)/mm:arch/$(ARCH)/kernel    \
                 :lib:fs:mm:kernel:testcases:init:sound:./$(OUTDIR)

SRCS          := init.c common.c vga.c vsprintf.c clib.c     \
                 gdt.c idt.c isr.c apic.c timer.c paging.c  \
	         sysinfo.c kdebug.c bootmem.c constants.c   \
                 cpu.c kbd-handler.c sched.c process.c idle.c 	\
		pgfault.c

TESTSRCS      := tests.c t_bootmem.c t_sprintf.c 

ASMSRCS       := boot.S kernel.S vectors.S kbd.S cpu-id.S utils.S

OBJS          := $(patsubst %.c,%.o,$(SRCS))
ASMOBJS       := $(patsubst %.S,%.o,$(ASMSRCS))
TESTOBJS      := $(patsubst %.c,%.o,$(TESTSRCS))

DEPENDS	      := $(patsubst %.c,.%.d,$(SRCS))
DEPENDS	      += $(patsubst %.S,.%.d,$(ASMSRCS))
DEPENDS       += $(patsubst %.c,.%.d,$(TESTSRCS))

BIN           := ganoid-$(VERSION)
CC            := gcc -g -c -std=gnu99 
CPPFLAGS      := -Wa,-march=i686 -mtune=generic -Wall -Iinclude -Iarch/$(ARCH)/include -fno-stack-protector -ffreestanding -O0 -Wextra -Wundef -Wshadow -Wunsafe-loop-optimizations  -Wpointer-arith -Wbad-function-cast -Wcast-qual -Wwrite-strings -Wconversion -Wsign-compare -Waddress -Waggregate-return -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wmissing-noreturn -Wunreachable-code -Winline -Wvolatile-register-var -Wpointer-sign

#AS	      := as	
#ASFLAGS	      := -march=i686  	


#.DEFAULT_GOAL = $(BIN)

$(BIN): $(OBJS) $(ASMOBJS) $(TESTOBJS) $(DEPENDS) Makefile  
	@printf "\n\nBuilding final executable. Date: "
	@date 
	@printf "\n"
	@cp $(LINKER_SCRIPT) bin
	$(MAKE) -C bin $(BIN)


%.o: %.c
	$(CC) $(CPPFLAGS) -o $(OUTDIR)/$@ $<

%.o: %.S
	$(CC) $(CPPFLAGS) -o $(OUTDIR)/$@ $<

.%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.%.d: %.S
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


include $(DEPENDS)

clean:
	@echo Cleaning ... Please wait
	@rm -f  $(OUTDIR)/*.o  $(OUTDIR)/$(BIN) 
	@echo Done


distclean:
	@echo Cleaning ... Please wait
	@rm -f \.*.d $(OUTDIR)/*.o  $(OUTDIR)/$(BIN)
	@echo Done


#common.o common.d: common.c
