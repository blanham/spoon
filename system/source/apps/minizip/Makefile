include ../../includes.make

INSTALLPATH = /spoon/system/bin/

UNZ_FILES = miniunz.c unzip.c ioapi.c 
ZIP_FILES = minizip.c zip.c   ioapi.c 

UNZ_OBJS = miniunz.o unzip.o ioapi.o 
ZIP_OBJS = minizip.o zip.o   ioapi.o 



all:	
	gcc $(HEADERPATH) $(CFLAGS)  -c $(UNZ_FILES) 
	#gcc $(HEADERPATH) $(CFLAGS)  -c $(ZIP_FILES) 
	ld $(UNZ_OBJS) $(LDFLAGS)  -o unzip
	#ld $(ZIP_OBJS) $(LDFLAGS)  -o zip

compile: all
	@echo done
	

clean:
	rm -f ./*.o
	#rm -f ./zip
	rm -f ./unzip

install:
	#cp -f ./zip  $(INSTALLPATH)/zip
	cp -f ./unzip  $(INSTALLPATH)/unzip

uninstall:
	#rm -f $(INSTALLPATH)/zip
	rm -f $(INSTALLPATH)/unzip


