
SUBDIRS =  shell terminal mkfs.djfs 			\
			desktop mpeg2decode untgz minizip	\
			pngview	commander					\
			ktest utest							\

#mpeg2decode minizip untgz cdplayer 


all:
	@echo "compile,clean,install,uninstall"
	

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



