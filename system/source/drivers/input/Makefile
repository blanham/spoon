

SUBDIRS = kbd ps2mouse


all:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) ; )  


compile:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) compile; )  


clean:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) clean; )

install:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) install; )


uninstall:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) uninstall; )






