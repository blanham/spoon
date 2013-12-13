include config/make.system
include config/make.configure
include config/make.$(ARCH)


LDSCRIPT=arch/$(ARCH_BASE)/kernel.ld


DEFINES = -D_$(ARCH_BASE)_ -D_VERSION_="$(VERSION)" 
DEFINES+= -D_VERSION_STR_="\"$(VERSION)\""
DEFINES+= -D_ARCH_STR_="\"$(ARCH)\""

INCLUDES = -I./include/ -I./
FLAGS = -nostartfiles -nostdlib -nodefaultlibs \
		-nostdinc  -fno-builtin   

LINKER_SCRIPT = -Xlinker --script=$(LDSCRIPT) 

GENERAL = -O0 -static -Wall

CFLAGS = $(DEFINES) $(INCLUDES) $(FLAGS) \
		 $(LINKER_SCRIPT) $(GENERAL) \
		 $(ARCH_CFLAGS) 

FILES = $(wildcard *.c)
FILES+= $(wildcard smk/*.c)
FILES+= schedulers/$(SCHEDULER)
FILES+= $(wildcard arch/$(ARCH_BASE)/*.c)
FILES+= $(wildcard arch/$(ARCH_BASE)/*.S)


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



