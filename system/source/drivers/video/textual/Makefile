include ../../../includes.make


FILES = *.cpp
INSTALLPATH = /spoon/system/drivers/video


all:	
	g++ $(HEADERPATH) $(CCFLAGS) -c $(FILES) 
	ld *.o  $(LDFLAGS) -o textual 


compile: all
	@echo done
	

clean:
	rm -f ./*.o
	rm -f ./textual


install:
	mkdir -p $(INSTALLPATH)
	cp -f textual $(INSTALLPATH)/textual

uninstall:
	rm -f $(INSTALLPATH)/textual



