include ../../includes.make


INSTALLPATH = /spoon/system/bin/

FILES= *.cpp


all:	
	echo $(CCFLAGS)
	g++ $(HEADERPATH) $(CCFLAGS)  -c $(FILES) 
	ld *.o $(LDFLAGS)  -o pngview

compile: all
	@echo done
	

clean:
	rm -f ./*.o
	rm -f ./pngview
	
install:
	cp -f ./pngview  $(INSTALLPATH)/pngview

uninstall:
	rm -f $(INSTALLPATH)/pngview


