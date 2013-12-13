include ../../includes.make



FILES = png.c pngset.c pngget.c pngrutil.c pngtrans.c pngwutil.c \
	pngread.c pngrio.c pngwio.c pngwrite.c pngrtran.c \
	pngwtran.c pngmem.c pngerror.c pngpread.c \


CFLAGS = $(HEADERPATH) 	\
		 -Wall 								\
		 -std=c99							\
		 -nostdlib -nostdinc -fno-builtin 	\
		 -O2 $(ARCH_CFLAGS)					


all:
	@echo "clean,compile,uninstall,install"

test:
	gcc $(CFLAGS) -c pngtest.c   
	ld  pngtest.o $(LDFLAGS) -o pngtest
	rm -f ./pngtest.o
	cp -f ./pngtest /spoon/system/bin/
	cp -f ./pngtest.png /spoon/system/media/


compile: clean
	gcc $(CFLAGS) -DNO_vsnprintf -c $(FILES)
	ar -rcv libpng.a  *.o

clean:
	rm -f ./*.o
	rm -f ./*.a
	rm -f ./pngtest

install: install_headers
	cp -f ./libpng.a /spoon/system/libraries


install_headers:
	cp -f ./png.h /spoon/system/source/include/
	cp -f ./pngconf.h /spoon/system/source/include/


uninstall:
	rm -f /spoon/system/libraries/libpng.a
	rm -f /spoon/system/source/include/png.h
	rm -f /spoon/system/source/include/pngconf.h
	rm -f /spoon/system/bin/pngtest
	rm -f /spoon/system/media/pngtest.png
	


