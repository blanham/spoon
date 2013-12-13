include ../../../includes.make


FILES = *.cpp
INSTALLPATH = /spoon/system/drivers/input


compile:	
	g++ $(HEADERPATH) $(CCFLAGS) -c $(FILES) 
	ld *.o $(LDFLAGS) -o kbd
	
clean:
	rm -f ./*.o
	rm -f ./kbd

install:
	mkdir -p $(INSTALLPATH)
	cp -f ./kbd  $(INSTALLPATH)/kbd


uninstall:
	rm -f $(INSTALLPATH)/kbd
	


