# Makefile for the UNIX library.
# Please see LICENSE for licensing information.
# 


# ---------	CURRENTLY SUPPORTED PLATFORMS ---------- 

PLATFORMS=`find ./os/ -type d | sed "s/.*\///" | sort`

# --------- FLAGS AND VARIABLES --------------------

CFLAGS=-O2 -nostdlib -nodefaultlibs -nostdinc -fno-builtin -static -Wall 
CCFLAGS=-O2 -nostdlib -nodefaultlibs -nostdinc -fno-builtin -static -Wall 
HEADERPATH=-I./include/unix/



# ---------  GENERIC MAKE RULES --------------------

all:
	@echo "Makefile for the UNIX library."
	@echo "Please see LICENSE for licensing information."
	@echo 
	@echo "Include files: ./include/unix/ "
	@echo "Output should be: libunix.a "
	@echo 
	@echo "Usage: make [ all | clean | <platform> ] "
	@echo 
	@echo "Currently supported platforms:"
	@echo
	@echo $(PLATFORMS)
	@echo
	@echo


clean:
	rm -f ./*.o
	rm -f ./*.a


# -------------- OS DEPENDENT BUILDS ---------------------------

spoon:
	gcc -D__x86__$(HEADERPATH) -I/spoon/system/source/include/ -I./os/$@/ $(CFLAGS) *.c support/*.c sys/*.c ./os/$@/*.c -c
	g++ -D__x86__ $(HEADERPATH) -I/spoon/system/source/include/ -I./os/$@/ $(CCFLAGS)./os/$@/*.cpp -c
	ar -rcv libunix.a  *.o

%: 
	@if [ ! -d ./os/$@ ]; then echo "There is no such platform. Supported platforms are: $(PLATFORMS)"; exit 255; fi;
	gcc $(HEADERPATH) -I./os/$@/ $(CFLAGS) *.c support/*.c sys/*.c ./os/$@/*.c -c
	#gcc -D__x86__ $(HEADERPATH) -I./os/$@/ $(CFLAGS) *.c sys/*.c ./os/$@/*.c -c
	ar -rcv libunix.a  *.o




