include ../../../includes.make


FILES = *.cpp
INSTALLPATH = /spoon/system/drivers/network/


compile:
	g++ $(HEADERPATH) $(CCFLAGS) -c $(FILES) 
	ld *.o $(LDFLAGS) -o rtl8139
	
clean:
	rm -f ./*.o
	rm -f ./rtl8139

install:
	mkdir -p $(INSTALLPATH)
	cp -f rtl8139 $(INSTALLPATH)/
	sync

uninstall:
	rm -f $(INSTALLPATH)/rtl8139

