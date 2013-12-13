include ../../../includes.make


FILES = *.cpp

INSTALLPATH = /spoon/system/drivers/storage/

compile: 
	g++ $(HEADERPATH) $(CCFLAGS)   -c $(FILES) 
	ld *.o $(LDFLAGS) -o ramdisk

bootdisk: disk.o
	g++ -I$(HEADERPATH) $(CCFLAGS) -D_BOOTDISK_  -c $(FILES) 
	ld *.o $(LDFLAGS) -o ramdisk 

disk.o: memdisk
	./convert.sh

clean:
	rm -f ./*.o
	rm -f ./ramdisk

install:
	cp -f ramdisk $(INSTALLPATH)/
	sync

uninstall:
	rm -f $(INSTALLPATH)/ramdisk

mount:
	mkdir tmpdir
	mount -o loop ./memdisk ./tmpdir

umount:
	umount ./tmpdir
	rmdir ./tmpdir
	touch ./memdisk

image:
	dd if=/dev/zero of=./memdisk bs=1024 count=1024

	
