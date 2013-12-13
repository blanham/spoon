include ../../../includes.make


FILES = *.cpp
INSTALLPATH = /spoon/system/drivers/video


all:	
	g++ $(HEADERPATH) $(CCFLAGS) -c $(FILES) 
	ld *.o  $(LDFLAGS) -o vmware 


compile: all
	@echo done
	

clean:
	rm -f ./*.o
	rm -f ./vmware


install:
	mkdir -p $(INSTALLPATH)
	cp -f vmware $(INSTALLPATH)/vmware

uninstall:
	rm -f $(INSTALLPATH)/vmware



