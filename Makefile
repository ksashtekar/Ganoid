#Makefile for the Ganoid kernel

VERSION = 0.0.1


BIN = ganoid-$(VERSION)
CC = gcc
CPPFLAGS = -Wall -I../include -fno-stack-protector -ffreestanding -O0

SUBDIRS = kernel arch drivers fs init lib mm sound

$(BIN): dummy ganoidsubdirs

ganoidsubdirs: dummy
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i; done

dummy:

