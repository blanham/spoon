include /spoon/system/source/includes.make


all:
	@echo "compile,install,uninstall,clean"


compile:	
	gcc $(HEADERPATH) $(CCFLAGS) -c *.c 
	ld *.o $(LDFLAGS) -o init


clean:
	rm -f ./*.o
	rm -f ./init


install:
	mkdir -p /spoon/system/
	cp -f ./init /spoon/system/init


uninstall:
	rm -f /spoon/system/init

