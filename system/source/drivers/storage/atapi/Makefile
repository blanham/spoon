include ../../../includes.make


FILES = *.cpp 

INSTALLPATH = /spoon/system/drivers/storage/

compile:	
	g++ $(HEADERPATH) $(CCFLAGS) -c $(FILES) 
	ld *.o $(LDFLAGS) -o atapi 
	
clean:
	rm -f ./*.o
	rm -f ./atapi

install:
	cp -f ./atapi $(INSTALLPATH)/atapi
	sync

uninstall:
	rm -f $(INSTALLPATH)/atapi

