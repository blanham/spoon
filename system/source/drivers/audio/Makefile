
SUBDIRS = 


all:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) all ; )  

compile:
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) compile ; )  

clean:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) clean; )

install:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) install; )


uninstall:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) uninstall; )





