# Makefile for the VFS library.
# Please see LICENSE for licensing information.
# 


# ---------	CURRENTLY SUPPORTED PLATFORMS ---------- 

# --------- FLAGS AND VARIABLES --------------------

CFLAGS=-O2 -nostdlib -nodefaultlibs -nostdinc -fno-builtin -static -Wall 
CCFLAGS=-O2 -nostdlib -nodefaultlibs -nostdinc -fno-builtin -static -Wall 
HEADERPATH=-I./include/vfs/ -I/spoon/system/source/include/



# ---------  GENERIC MAKE RULES --------------------

all: compile
	@echo "done"


clean:
	rm -f ./*.o
	rm -f ./*.a

compile:
	gcc $(HEADERPATH) $(CFLAGS) *.c -c
	ar -rcv libvfs.a  *.o



