include ../../../includes.make


FILES = main.cpp PS2Driver.cpp	
INSTALLPATH = /spoon/system/drivers/input


compile:	
	g++ $(HEADERPATH)  $(CCFLAGS) -c $(FILES) 
	ld *.o $(LDFLAGS) -o ps2mouse 
	
clean:
	rm -f ./*.o
	rm -f ./ps2mouse

install:
	cp -f ./ps2mouse  $(INSTALLPATH)/ps2mouse
	sync

uninstall:
	rm -f $(INSTALLPATH)/ps2mouse

