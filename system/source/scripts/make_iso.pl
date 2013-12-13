#!/usr/bin/perl -w

#  -----------------------------

use strict;


my $BUILD_DIR = "/spoon/system/source/builds";


my $COMMAND = "/usr/bin/mkisofs -f -l -A \"spoon install CD\"  -V \"spoon install CD\" -root /spoon  -b spoon/system/floppy.img -o $BUILD_DIR/cdrom.iso -x /spoon/_darcs -x /spoon/system/source  /spoon"; 


# -----------------------------



die "mkisofs not found in /usr/bin/mkisofs\n" 	if ( ! -e "/usr/bin/mkisofs" );

die "floppy.img not found in correct path ($BUILD_DIR).\n" .
    "Run make_floppy.pl in the scripts directory first.\n" 	
	if ( (! -e "$BUILD_DIR/floppy.img") );
	
# Make the iso image image

  system("cp -f $BUILD_DIR/floppy.img /spoon/system/" );

  die "mkisofs failed! dunno why.\n" if 
  ( system($COMMAND) != 0 );

  system("rm -f /spoon/system/floppy.img" );

print "\n";
print "****************************************************\n";
print "You should be able to boot the iso now and use\n";
print "the system. As always, let me know if you have any\n";
print "problems or just tell me what you think.\n\n";
print "\n";
print "good luck.\n"


