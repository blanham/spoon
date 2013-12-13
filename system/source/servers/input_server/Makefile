include ../../includes.make


FILES = *.cpp
INSTALLPATH = /spoon/system/servers/


all:	
	g++ $(HEADERPATH) $(CCFLAGS) -c $(FILES) 
	ld *.o $(LDFLAGS) -o input_server

compile: all
	@echo done
	

clean:
	rm -f ./*.o
	rm -f ./input_server

install:
	cp -f ./input_server  $(INSTALLPATH)/input_server

uninstall:
	rm -f $(INSTALLPATH)/input_server


