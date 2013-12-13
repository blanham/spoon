include ../../includes.make


FILES = *.cpp
INSTALLPATH = /spoon/system/bin/


all:	
	g++ $(HEADERPATH) $(CCFLAGS)  -c $(FILES) 
	ld *.o $(LDFLAGS)  -o commander

compile: all
	@echo done
	

clean:
	rm -f ./*.o
	rm -f ./commander

install:
	cp -f ./commander  $(INSTALLPATH)/commander

uninstall:
	rm -f $(INSTALLPATH)/commander


