VERSION=snapshot
ARCH=x86

LDSCRIPT=kernel.ld


DEFINES = -D_VERSION_="$(VERSION)" 
DEFINES+= -D_ARCH_="\"$(ARCH)\""

INCLUDES= -I./include/

FLAGS = -nostartfiles -nostdlib -nodefaultlibs \
		-nostdinc  -fno-builtin   

LINKER_SCRIPT = -Xlinker --script=$(LDSCRIPT) 

GENERAL = -O0 -static -Wall

CFLAGS = $(DEFINES) $(INCLUDES) $(FLAGS) \
		 $(LINKER_SCRIPT) $(GENERAL)

FILES = $(wildcard *.c)
FILES+= $(wildcard ds/*.c)
FILES+= $(wildcard *.S)


compile:   
	$(CC) $(CFLAGS) -o kernel $(FILES)


all:
	@echo "compile,clean,install,uninstall"

clean:
	rm -f kernel

install:
	cp -f ./kernel  /spoon/system/kernel

uninstall:
	rm -f /spoon/system/kernel



