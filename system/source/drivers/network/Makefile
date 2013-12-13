
SUBDIRS = 


all:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) ; )  

clean:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) clean; )
	$(RM) ./data

install:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) install; )


uninstall:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) uninstall; )


compile:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) compile; )



