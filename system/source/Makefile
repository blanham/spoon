

help:    
	@echo "IMPORTANT: You need to set up the symbolic link \"/spoon\" to point to" 
	@echo "the base path of the spoon file system tree." 
	@echo
	@echo "example: ln -s /home/john/spoon /spoon" 
	@echo
	@echo
	@echo "Options: "
	@echo "           all - compile the whole system and install"
	@echo "     libraries - compile the libraries and installs them. "
	@echo "        kernel - compile the kernel"
	@echo "          init - compile init"
	@echo "       drivers - compile the drivers"
	@echo "       servers - compile the servers"
	@echo "          apps - compile the applications"
	@echo
	@echo " documentation - compile the documentation (requires doxygen)"
	@echo 
	@echo "       install - install everything"
	@echo "     uninstall - uninstall everything"
	@echo


all:	checks
	make -C libraries compile
	make -C libraries install
	make -C kernel compile
	make -C kernel install
	make -C init compile
	make -C init install
#	make -C drivers compile
#	make -C drivers install
#	make -C servers compile
#	make -C servers install
#	make -C apps compile
#	make -C apps install


libraries: checks
	make -C libraries compile
	make -C libraries install
	
kernel: checks
	make -C kernel compile

init: checks
	make -C init compile


drivers: checks
	make -C drivers compile
	
servers: checks
	make -C servers compile

apps: checks
	make -C apps compile

documentation: checks
	make -C Documentation compile

install:
	make -C libraries install
	make -C kernel install
	make -C init install
#	make -C drivers install
#	make -C servers install
#	make -C apps install

uninstall:
	make -C libraries uninstall
	make -C kernel uninstall
	make -C init uninstall
#	make -C drivers uninstall
#	make -C servers uninstall
#	make -C apps uninstall

clean:
	make -C libraries clean
	make -C kernel clean
	make -C init clean
#	make -C drivers clean
#	make -C servers clean
#	make -C apps clean
	make -C Documentation clean
	rm -f ./builds/*
	rm -f ./bochs.log


checks:
	@sh -c "\
	if [ ! -e /spoon/system/libraries/libstdc++.a  ]; then\
	  echo \"For the dynamic runtime typing of C++, you\";\
	  echo \"need to have libstdc++.a copied into:\";\
	  echo;\
	  echo \"    /spoon/system/libraries/libstdc++.a\";\
	  echo ;\
	  echo \"Please copy your copy into it.\";\
	  exit -1;\
	fi;\
	"

bochs: all install run


run: 
	cd kernel && make && make install
	cd scripts && sudo ./make_floppy.pl
	/spoon/system/source/thirdparty/bochs/bin/bochs -f ./config/bochs.cfg

