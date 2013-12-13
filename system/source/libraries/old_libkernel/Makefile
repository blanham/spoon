include ./config/make.configure
include ./config/make.$(ARCH)


FILES = *.c arch/$(ARCH_BASE)/*.c arch/$(ARCH_BASE)/*.S

FILES = *.c arch/$(ARCH_BASE)/*.c arch/$(ARCH_BASE)/*.S

CFLAGS = -I./include/ 					 	\
		 -Wall 								\
		 -nostdlib -nostdinc -fno-builtin 	\
		 -O0 $(ARCH_CFLAGS)					


all:
	@echo "clean,compile"

compile: clean
	gcc $(CFLAGS) -c $(FILES)
	ar -rcv libkernel.a  *.o

clean:
	rm -f ./*.o
	rm -f ./*.a


