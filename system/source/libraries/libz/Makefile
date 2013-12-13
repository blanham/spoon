include ../../includes.make


FILES = adler32.c compress.c crc32.c gzio.c uncompr.c deflate.c trees.c \
       zutil.c inflate.c infback.c inftrees.c inffast.c

CFLAGS = $(HEADERPATH) 	\
		 -Wall 								\
		 -std=c99							\
		 -nostdlib -nostdinc -fno-builtin 	\
		 -O0 $(ARCH_CFLAGS)					


all:
	@echo "clean,compile,uninstall,install"



compile: clean
	gcc $(CFLAGS) -DNO_vsnprintf -c $(FILES)
	ar -rcv libz.a  *.o

clean:
	rm -f ./*.o
	rm -f ./*.a

install: install_headers
	cp -f ./libz.a /spoon/system/libraries


install_headers:
	cp -f ./zlib.h /spoon/system/source/include/
	cp -f ./zconf.h /spoon/system/source/include/


uninstall:
	rm -f /spoon/system/libraries/libz.a
	rm -f /spoon/system/source/include/zconf.h
	rm -f /spoon/system/source/include/zlib.h
	


