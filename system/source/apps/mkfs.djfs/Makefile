include ../../includes.make


FILES = *.cpp 
INSTALLPATH = /spoon/system/bin/

all:	
	g++ $(HEADERPATH) -nostdinc  $(CCFLAGS)  -c $(FILES) 
	ld  *.o $(LDFLAGS) -o mkfs.djfs

compile: all
	@echo done
	

clean:
	rm -f ./*.o
	rm -f ./mkfs.djfs

install:
	cp -f ./mkfs.djfs  $(INSTALLPATH)/mkfs.djfs

uninstall:
	rm -f $(INSTALLPATH)/mkfs.djfs


