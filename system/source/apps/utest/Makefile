include ../../includes.make


FILES = *.cpp 
INSTALLPATH = /spoon/system/bin/

all:	
	g++ $(HEADERPATH)  $(CCFLAGS)  -c $(FILES) 
	ld  *.o $(LDFLAGS) -o utest

unix:
	g++ *.cpp -o utest

compile: all
	@echo done
	

clean:
	rm -f ./*.o
	rm -f ./utest

install:
	cp -f ./utest  $(INSTALLPATH)/utest

uninstall:
	rm -f $(INSTALLPATH)/utest


