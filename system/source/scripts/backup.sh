#!/bin/sh


# ---------------------------------------------
#  This is an automatic backup script which
#  produces the following files as output:
#
#     smk.tar.gz & smk-$DATE.tar.gz
#     libunix.tar.gz & libunix-$DATE.tar.gz
#     spoon.tar.gz & spoon-$DATE.tar.gz
#
#  Where $DATE is %Y%m%d-%H%M%S and is generated
#  using the standard date command.
#
#  It uses the $TEMPDIR for working.
#  It uses the $BACKUPDIR for storage of the
#  output files.
#
# ---------------------------------------------

DATE=`date +"%Y%m%d-%H%M%S"`

TARFLAGS=czf


SPOONNAME=spoon-$DATE.tar.gz
SMKNAME=smk-$DATE.tar.gz
UNIXNAME=libunix-$DATE.tar.gz
LIBALLOCNAME=liballoc-$DATE.tar.gz

TEMPDIR=/tmp/spoon
BACKUPDIR=~/backups

# --------------------------------------------------------

# Request permission to do everything.

echo "Does this script have permission to do the following:";
echo;
echo " 1. Create the directories $BACKUPDIR and $TEMPDIR if";
echo "    non-existant.";
echo " 2. Remove everything in $TEMPDIR";
echo " 3. Create backups in $BACKUPDIR";
echo
echo " yes or no [y/N] ? "
read PERMISSION

GOOD=n;

if [ "$PERMISSION" == "Y" ]; then GOOD=Y; fi;
if [ "$PERMISSION" == "y" ]; then GOOD=Y; fi;

if [ "$GOOD" != "Y" ]; then
	echo "Okay. No permission given. I won't do anything.";
	exit 0;
fi;

# ---------------------------------------------------------

# Create the directories

rm -rf $TEMPDIR
mkdir -p $TEMPDIR
mkdir -p $BACKUPDIR

# Check to see if it worked.

if [ ! -d $BACKUPDIR ]; then
	echo "Unable to create: $BACKUPDIR"
	exit -1;
fi;

if [ ! -d $TEMPDIR ]; then
	echo "Unable to create: $TEMPDIR"
	exit -1;
fi;


# ---------------------------------------------------------


# Clean the source tree

cd /spoon/system/source
make clean
make uninstall


# For the smk directory.
cp -rf /spoon/system/source/kernel \
	   /spoon/system/source/libraries/libkernel \
	   /spoon/system/source/ldscripts \
	   /spoon/system/source/LICENSE \
	   $TEMPDIR/



tar -$TARFLAGS $BACKUPDIR/snapshot.tar.gz --exclude=/spoon/_darcs --exclude=/spoon/system/media/* --exclude=/spoon/system/source/thirdparty /spoon
tar -$TARFLAGS $BACKUPDIR/libunix.tar.gz -C /spoon/system/source/libraries/  ./unix
tar -$TARFLAGS $BACKUPDIR/liballoc.tar.gz -C /spoon/system/source/libraries/  ./liballoc
tar -$TARFLAGS $BACKUPDIR/smk.tar.gz -C $TEMPDIR/ \
			./libkernel ./kernel ./ldscripts \
			./LICENSE


# ---------------------------------------------------------
# Rename the files.

cp -f $BACKUPDIR/snapshot.tar.gz $BACKUPDIR/$SPOONNAME
cp -f $BACKUPDIR/libunix.tar.gz $BACKUPDIR/$UNIXNAME
cp -f $BACKUPDIR/liballoc.tar.gz $BACKUPDIR/$UNIXNAME
cp -f $BACKUPDIR/smk.tar.gz $BACKUPDIR/$SMKNAME

# ---------------------------------------------------------
# Clean the temporary directory.
rm -rf $TEMPDIR

# ---------------------------------------------------------

echo "";
echo "The following files were created: ";
echo 
echo "$BACKUPDIR/smk.tar.gz"
echo "$BACKUPDIR/libunix.tar.gz"
echo "$BACKUPDIR/liballoc.tar.gz"
echo "$BACKUPDIR/snapshot.tar.gz"
echo 
echo "$BACKUPDIR/$SMKNAME"
echo "$BACKUPDIR/$UNIXNAME"
echo "$BACKUPDIR/$LIBALLOCNAME"
echo "$BACKUPDIR/$SPOONNAME"
echo 
echo "done.";



