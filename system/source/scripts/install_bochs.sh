#!/bin/bash


LASTPATH=`pwd`


BOCHS=bochs-2.2.6
BOCHSURL=http://heanet.dl.sourceforge.net/sourceforge/bochs/$BOCHS.tar.gz
TEMPDIR=/tmp/spoon-$BOCHS
INSTALLPATH=/spoon/system/source/thirdparty/bochs



CONFIGSTRING="--prefix=$INSTALLPATH  --enable-sep --enable-smp --enable-apic --enable-vbe --enable-4meg-pages --enable-global-pages --enable-cpu-level=6"


function doInstall()
{
	
	mkdir -p $TEMPDIR && \
	cd $TEMPDIR && \
	wget -c $BOCHSURL && \
	tar -xzvf $BOCHS.tar.gz && \
	cd $BOCHS && \
	./configure $CONFIGSTRING && \
	make && \
	make install && \
	cd .. && \
	cp -f $BOCHS.tar.gz $INSTALLPATH && \
	cd $LASTPATH && \
	rm -rf $TEMPDIR 

}



case "$1" in

	install)
		if [ ! -e $INSTALLPATH ]; then
			doInstall
		else
			echo "It seems like you already have it installed. If this is"
			echo "not the case, delete the installation directory."
			echo "installation directory: $INSTALLPATH"
		fi
		exit
	;;
	force)
		doInstall
		exit
	;;
	uninstall)
		rm -rf $INSTALLPATH
		exit
	;;

#---------------------------------
# Usage message
#---------------------------------

	*)
	  echo "usage: install_bochs.sh [ install | force | uninstall ]"
	  echo
	  echo "This script downloads bochs, extracts it to a temp directory,"
  	  echo "compiles it and installs it into the correct path. Here are"
	  echo "the URLS and paths:"
	  echo
	  echo "    bochs URL          - $BOCHSURL" 
	  echo "    temp direcory      - $TEMPDIR" 
	  echo "    installation path  - $INSTALLPATH"
	  echo 
	  echo "Configuration string: $CONFIGSTRING"
 	  exit
	;;
esac

# --------------------------------------------------------






