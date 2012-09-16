# Check whether configuration file is present ...
CONFIG_FILE := .config
CONFIG_FILE_PRESENT := $(wildcard $(CONFIG_FILE))
AUTOCONF_FILE := include/autoconf.h

#.DEFAULT_GOAL := all

# *******************************************************************************
ifeq ($(strip $(CONFIG_FILE_PRESENT)),)

menuconfig:
	@tools/mconf Kconfig

ifneq ($(MAKECMDGOALS),menuconfig)
$(error Unable to find $(CONFIG_FILE). Your tree is not configured)
endif # ! menuconfig

else # CONFIG_FILE_PRESENT


# The sequence of inclusion of below files is important.

#Makefile for the Ganoid kernel

include .config

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
	         sysinfo.c kdebug.c page.c constants.c   \
	         cpu.c kbd-handler.c sched.c process.c idle.c 	\
		 pgfault.c utils.c ganoid-acpi.c heap.c

include lib/Makefile


TESTSRCS      := tests.c t_sprintf.c 

ASMSRCS       := boot.S kernel.S vectors.S kbd.S cpu-id.S asm-utils.S

OBJS          := $(patsubst %.c,%.o,$(SRCS))
ASMOBJS       := $(patsubst %.S,%.o,$(ASMSRCS))
TESTOBJS      := $(patsubst %.c,%.o,$(TESTSRCS))

DEPENDS	      := $(patsubst %.c,.%.d,$(SRCS))
DEPENDS	      += $(patsubst %.S,.%.d,$(ASMSRCS))
DEPENDS       += $(patsubst %.c,.%.d,$(TESTSRCS))

BIN           := ganoid-$(VERSION)

WARN_FLAGS := -Wall -Wextra -Wundef -Wshadow -Wunsafe-loop-optimizations \
	 -Wpointer-arith -Wbad-function-cast \
	-Wwrite-strings -Wconversion -Wsign-compare -Waddress \
	-Waggregate-return -Wstrict-prototypes -Wmissing-prototypes \
	-Wmissing-declarations -Wmissing-field-initializers \
	-Wmissing-noreturn -Wunreachable-code -Winline -Wvolatile-register-var \
	-Wpointer-sign

# Enable this locally for extra non-default warnings
# WARN_FLAGS += -Wcast-qual


CPPFLAGS      := -Wa,-march=i686 -mtune=generic \
	-Iinclude -Iarch/$(ARCH)/include \
	-Iinclude/acpi -Iinclude/acpi/tools \
	-Iinclude/asm-generic \
	-include include/autoconf.h \
	-fno-stack-protector -ffreestanding -O0 --no-builtin $(WARN_FLAGS) \
	-nostdlib -nostdinc \
	-D__KERNEL__ -D_GANOID

#AS	      := as	
#ASFLAGS	      := -march=i686  	


ifneq ($(VERBOSE), 1)
.SILENT:

CC            := gcc -g -c -std=gnu99
else
CC            := gcc -g -c -std=gnu99
endif # VERBOSE

#.DEFAULT_GOAL = $(BIN)

$(BIN): $(AUTOCONF_FILE) $(OBJS) $(ASMOBJS) $(TESTOBJS) $(DEPENDS) Makefile
	@printf "\n\nBuilding final executable. Date: "
	@date 
	@printf "\n"
	@cp $(LINKER_SCRIPT) bin
	@$(MAKE) -C bin $(BIN)

$(AUTOCONF_FILE): $(CONFIG_FILE)
	@tools/autoconf $< $@

test: $(BIN)
	@tools/test-kernel `pwd` $(BIN) $(CONFIG_TEST_DIR) bigdisk.img

%.o: %.c
	@echo "CC  $(shell basename $<)"
	$(CC) $(CPPFLAGS) -o $(OUTDIR)/$@ $<

%.o: %.S
	@echo ASM $(shell basename $<)
	$(CC) $(CPPFLAGS) -o $(OUTDIR)/$@ $<

.%.d: %.c
	@echo "DEP $(shell basename $<)"
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.%.d: %.S
	@echo "DEP $(shell basename $<)"
	set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


-include $(DEPENDS)

clean:
	@echo Cleaning ... Please wait
	@rm -f  $(OUTDIR)/*.o  $(OUTDIR)/$(BIN) 
	@echo Done


distclean:
	@echo Cleaning ... Please wait
	@rm -f \.*.d $(OUTDIR)/*.o  $(OUTDIR)/$(BIN)
	@echo Done

menuconfig:
	@tools/mconf Kconfig


#common.o common.d: common.c

endif # ! CONFIG_FILE_PRESENT
