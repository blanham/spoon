include ../../../includes.make


FILES = *.cpp 
INSTALLPATH = /spoon/system/drivers/storage/


compile:	
	g++ $(HEADERPATH) $(CCFLAGS) -c $(FILES) 
	ld *.o $(LDFLAGS) -o ata 
	
clean:
	rm -f ./*.o
	rm -f ./ata

install:
	cp -f ./ata $(INSTALLPATH)/ata
	sync

uninstall:
	rm -f $(INSTALLPATH)/ata

