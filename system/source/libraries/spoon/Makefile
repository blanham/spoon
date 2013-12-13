include ../../includes.make

HEADERPATHS= -I/spoon/system/source/libraries/spoon/include \
			 -I/spoon/system/source/include \
			 -I/spoon/system/source/include/unix

export OBJDIR  = $(PWD)/objs
export CCFLAGS = $(HEADERPATHS) -Wall -nostdlib -nostdinc \
				 -fno-builtin -O0 -static

SUBDIRS = app base collections interface io ipc storage \
		  support video 

TRANSLATORS = translators 


all:	compile
	@echo "compile,clean,install,uninstall"


compile: clean 
	if [ -d $(OBJDIR) ]; then \
		rm -f $(OBJDIR)/*.o;	\
	else \
	   mkdir $(OBJDIR); \
	fi;
	$(foreach dir,$(SUBDIRS), \
	   $(MAKE) -s -C $(dir) compile ; )  
	ar -rcv libspoon.a  $(OBJDIR)/*.o
	rm -f $(OBJDIR)/*.o
	$(foreach dir,$(TRANSLATORS), \
	   $(MAKE) -s -C $(dir) compile ; )  
	ar -rcv libtranslators.a  $(OBJDIR)/*.o
	rm -rf $(OBJDIR)

clean:
	if [ -d $(OBJDIR) ]; then \
		rm -f $(OBJDIR)/*.o;	\
		rmdir $(OBJDIR);	\
	else \
	   mkdir $(OBJDIR); \
	fi;
	rm -f ./libspoon.a
	rm -f ./libtranslators.a


install: install_headers 
	cp -f ./libspoon.a $(LIBRARYPATH)/
	cp -f ./libtranslators.a $(LIBRARYPATH)/


install_headers:
	cp -rf ./include/* /spoon/system/source/include/


uninstall: 
	rm -f $(LIBRARYPATH)/libspoon.a
	rm -f $(LIBRARYPATH)/libtranslators.a
	rm -rf /spoon/system/source/include/spoon



