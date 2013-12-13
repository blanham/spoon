include ../../includes.make


FILES = *.cpp 
INSTALLPATH = /spoon/system/bin/

all:	
	g++ $(HEADERPATH)  $(CCFLAGS)  -c $(FILES) 
	ld  *.o $(LDFLAGS) -o ktest

compile: all
	@echo done
	

clean:
	rm -f ./*.o
	rm -f ./ktest

install:
	cp -f ./ktest  $(INSTALLPATH)/ktest

uninstall:
	rm -f $(INSTALLPATH)/ktest


