include ../../includes.make


FILES = *.cpp
INSTALLPATH = /spoon/system/servers/


all:	
	g++ $(HEADERPATH) $(CCFLAGS) -c $(FILES) 
	ld *.o $(LDFLAGS) -o app_server

compile: all
	@echo done
	

clean:
	rm -f ./*.o
	rm -f ./app_server

install:
	cp -f ./app_server  $(INSTALLPATH)/app_server

uninstall:
	rm -f $(INSTALLPATH)/app_server


