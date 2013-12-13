
SUBDIRS = textual vga vmware grub


all:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) ; )  

clean:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) clean; )

install:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) install; )

compile:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) compile; )

uninstall:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) uninstall; )









