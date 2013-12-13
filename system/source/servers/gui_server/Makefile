include ../../includes.make


MASKS = *.cpp video_drivers/*.cpp 
FILES = $(wildcard $(MASKS))
INSTALLPATH = /spoon/system/servers/


all:
	g++  $(HEADERPATH) -I./include/ $(CCFLAGS) -c $(FILES) 
	ld *.o $(LDFLAGS) -o gui_server


compile: all
	@echo done
	

clean:
	rm -f ./*.o
	rm -f ./gui_server

install:
	cp -f ./gui_server $(INSTALLPATH)/gui_server

uninstall:
	rm -f $(INSTALLPATH)/gui_server


