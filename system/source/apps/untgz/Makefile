include ../../includes.make


INSTALLPATH = /spoon/system/bin/

FILES= *.c


all:	
	gcc $(HEADERPATH) $(CCFLAGS)  -c $(FILES) 
	ld *.o $(LDFLAGS)  -o untgz

compile: all
	@echo done
	

clean:
	rm -f ./*.o
	rm -f ./untgz
	
install:
	cp -f ./untgz  $(INSTALLPATH)/untgz

uninstall:
	rm -f $(INSTALLPATH)/untgz


