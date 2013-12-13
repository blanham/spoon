include ../../includes.make


FILES = adler32.c compress.c crc32.c gzio.c uncompr.c deflate.c trees.c \
       zutil.c inflate.c infback.c inftrees.c inffast.c

CFLAGS = $(HEADERPATH) 	\
		 -Wall 								\
		 -nostdlib -nostdinc -fno-builtin 	\
		 -O0 $(ARCH_CFLAGS)					


all:
	@echo "clean,compile,uninstall,install"

compile: clean
	gcc $(CFLAGS) -c $(FILES)
	ar -rcv libz.a  *.o

clean:
	rm -f ./*.o
	rm -f ./*.a

install:
	cp -f ./libz.a /spoon/system/libraries

uninstall:
	rm -f /spoon/system/libraries/libz.a
	
