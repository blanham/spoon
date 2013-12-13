
FILES = *.c *.S

CFLAGS = -I./include/ 					 	\
		 -Wall 								\
		 -nostdlib -nostdinc -fno-builtin 	\
		 -O0 


all: clean compile

compile: clean
	gcc $(CFLAGS) -c $(FILES)
	ar -rcv libkernel.a  *.o

clean:
	rm -f ./*.o
	rm -f ./*.a


