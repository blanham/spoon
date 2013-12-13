include ../../../includes.make


FILES = *.cpp
INSTALLPATH = /spoon/system/drivers/video


all:	
	g++ $(HEADERPATH) $(CCFLAGS) -c $(FILES) 
	ld *.o  $(LDFLAGS) -o grub 


compile: all
	@echo done
	

clean:
	rm -f ./*.o
	rm -f ./grub


install:
	mkdir -p $(INSTALLPATH)
	cp -f grub $(INSTALLPATH)/grub

uninstall:
	rm -f $(INSTALLPATH)/grub



