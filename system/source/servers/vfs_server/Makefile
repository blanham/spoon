include ../../includes.make


FILES = *.cpp io/*.cpp primitives/*.cpp tables/*.cpp fs/*.cpp
INSTALLPATH = /spoon/system/servers/


all:	
	g++ $(HEADERPATH) $(CCFLAGS) -c  $(FILES) 
	ld *.o $(LDFLAGS) -o vfs_server

compile: all
	@echo done
	

clean:
	rm -f ./*.o
	rm -f ./vfs_server

install:
	cp -f ./vfs_server $(INSTALLPATH)/vfs_server

uninstall:
	rm -f $(INSTALLPATH)/vfs_server


