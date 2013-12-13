include ../includes.make

# This file has been changed recently to support unusual
# libraries and the new system layout.


#SUBDIRS= spoon libz libpng
#HEADS=  spoon libz libpng

all:
	@echo "compile,clean,install,uninstall"


compile:  install_headers
#	$(MAKE) -s -C vfs compile;
#	$(MAKE) -s -C unix spoon;
	$(MAKE) -s -C libkernel compile;
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) compile ; )  

clean:  
#	$(MAKE) -s -C vfs clean;
#	$(MAKE) -s -C unix clean;
	$(MAKE) -s -C libkernel clean;
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) clean; )

install:  
	mkdir -p $(LIBRARYPATH)
#	cp -f ./vfs/libvfs.a $(LIBRARYPATH)
#	cp -f ./unix/libunix.a $(LIBRARYPATH)
	cp -f ./libkernel/libkernel.a $(LIBRARYPATH)
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) install; )

uninstall:  
#	rm -rf /spoon/system/source/include/vfs
#	rm -rf /spoon/system/source/include/unix
	rm -rf /spoon/system/source/include/kernel
	rm -f /spoon/system/source/include/kernel.h
	rm -f $(LIBRARYPATH)/libunix.a 
	rm -f $(LIBRARYPATH)/libkernel.a
	rm -f $(LIBRARYPATH)/libvfs.a
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) uninstall; )


install_headers:
	mkdir -p /spoon/system/source/include/
#	cp -rf ./vfs/include/* /spoon/system/source/include/
#	cp -rf ./unix/include/* /spoon/system/source/include/
	cp -rf ./libkernel/include/* /spoon/system/source/include/
	$(foreach dir,$(HEADS), \
	   $(MAKE) -s -C $(dir) install_headers ; )  



