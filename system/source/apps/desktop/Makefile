include ../../includes.make


FILES = *.cpp
INSTALLPATH = /spoon/system/bin/


all:	
	g++ $(HEADERPATH) $(CCFLAGS)  -c $(FILES) 
	ld *.o $(LDFLAGS)  -o desktop

compile: all
	@echo done
	

clean:
	rm -f ./*.o
	rm -f ./desktop

install:
	cp -f ./desktop  $(INSTALLPATH)/desktop
	mkdir -p /spoon/system/media/desktop/
	cp -rf ./images/* /spoon/system/media/desktop/

uninstall:
	rm -f $(INSTALLPATH)/desktop
	rm -rf /spoon/system/media/desktop


