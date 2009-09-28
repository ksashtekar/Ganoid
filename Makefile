#Makefile for the Ganoid kernel

export

OUTDIR        := bin

VERSION       := 0.0.1	
ARCH          := i386
LINKER_SCRIPT := arch/$(ARCH)/kernel/ganoid.ld

VPATH         := arch/$(ARCH)/drivers:testcases:arch/$(ARCH)/boot: \
                 arch/$(ARCH)/lib:arch/$(ARCH)/mm:arch/$(ARCH)/kernel    \
                 :lib:fs:mm:kernel:init:sound:./$(OUTDIR)

SRCS          := common.c monitor.c vga.c vsprintf.c clib.c     \
                 gdt.c idt.c isr.c apic.c timer.c paging.c

ASMSRCS       := boot.S kernel.S vectors.S

OBJS          := $(patsubst %.c,%.o,$(SRCS))
ASMOBJS       := $(patsubst %.S,%.o,$(ASMSRCS))

DEPENDS	      := $(patsubst %.c,.%.d,$(SRCS))
DEPENDS	      += $(patsubst %.S,.%.d,$(ASMSRCS))


BIN           := ganoid-$(VERSION)
CC            := gcc -g -c
CPPFLAGS      := -Wall -Iinclude -Iarch/$(ARCH)/include -fno-stack-protector -ffreestanding -O0


#.DEFAULT_GOAL = $(BIN)

$(BIN): $(OBJS) $(ASMOBJS) $(DEPENDS) Makefile  
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
	rm -vf .*.d  $(OUTDIR)/*.o  $(OUTDIR)/$(BIN) 


#common.o common.d: common.c
