
SUBDIRS = app_server input_server vfs_server gui_server 



all:
	@echo "clean,compile,install,uninstall"

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





