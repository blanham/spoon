
SUBDIRS = #input storage network fs 
SUBDIRS = network input storage video 


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
	cp -f ./driver.map /spoon/system/drivers/

uninstall:  
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) uninstall; )
	rm -f /spoon/system/drivers/driver.map
	






