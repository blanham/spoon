include ../../includes.make


FILES = functions/*.cpp *.cpp 
INSTALLPATH = /spoon/system/bin/

all:	
	g++ $(HEADERPATH)  $(CCFLAGS)  -c $(FILES) 
	ld  *.o $(LDFLAGS) -o shell

compile: all
	@echo done
	

clean:
	rm -f ./*.o
	rm -f ./shell

install:
	cp -f ./shell  $(INSTALLPATH)/shell

uninstall:
	rm -f $(INSTALLPATH)/shell


