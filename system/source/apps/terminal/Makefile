include ../../includes.make


FILES = *.cpp
INSTALLPATH = /spoon/system/bin/


all:	
	g++ $(HEADERPATH) $(CCFLAGS)  -c $(FILES) 
	ld *.o $(LDFLAGS)  -o terminal

compile: all
	@echo done


clean:
	rm -f ./*.o
	rm -f ./terminal


install:
	cp -f ./terminal  $(INSTALLPATH)/terminal


uninstall:
	rm -f $(INSTALLPATH)/terminal


