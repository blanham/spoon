#!/usr/bin/perl -w

#  I think this is going to be a lot of work for something
#  which will be constantly changing.. let's hope this works
#  well enough.

use strict;
use File::Copy;

my $BUILD_DIR = "/spoon/system/source/builds";


die "You need to be root.\n" if ($ENV{'USER'} ne "root");

die "GRUB not found at /sbin/grub or ./grub\n" 	
	if ( (! -e "/sbin/grub") && (! -e "./grub") );
	
die "dd not found in /bin/dd\n" 		if ( ! -e "/bin/dd" );
die "mkfs not found in /sbin/mkfs\n" 	if ( ! -e "/sbin/mkfs" );
die "mount not found in /bin/mount\n" 	if ( ! -e "/bin/mount" );
die "umount not found in /bin/umount\n" if ( ! -e "/bin/umount" );
die "gzip not found in /bin/gzip\n" 	if ( ! -e "/bin/gzip" );

die "./tmp.floppy directory already exists! Please unmount it and delete it first\n" if ( -e "./tmp.floppy" );

die "/boot/stage1, /boot/grub/stage1 or ./stage1 not present\n"
   if ( (! -e "/boot/stage1") &&  (! -e "/boot/grub/stage1") &&
	(! -e "./stage1") );

die "/boot/stage2, /boot/grub/stage2 or ./stage2 not present\n"
   if ( (! -e "/boot/stage2") && (! -e "/boot/grub/stage2") &&
	(! -e "./stage2") );

# Just a quick, silly test to see if the user did a make all

die "/spoon/system/kernel not found. Did you do a \"make all\" in the main source root?\n"
	if ( ! -e "/spoon/system/kernel" );


# Make the floppy image

  die "dd /dev/zero for 1.44M to $BUILD_DIR/floppy.img failed!\n" if 
  ( system("/bin/dd bs=1 count=1474560 if=/dev/zero of=$BUILD_DIR/floppy.img") != 0 );
  die "making ext2 on $BUILD_DIR/floppy.img failed!\n" if 
  ( system("echo -e \"y\\n\" | /sbin/mkfs -t ext2 $BUILD_DIR/floppy.img") != 0 );
  mkdir( "./tmp.floppy" );
  die "unable to mount $BUILD_DIR/floppy.img onto ./tmp.floppy" if 
  ( system("/bin/mount -t ext2 -o loop $BUILD_DIR/floppy.img ./tmp.floppy") != 0 );


my $STATE	=	0;
my $GRUB_CLEAN	=	0;


  my $fd;
  	open($fd,"< ../config/floppy.conf") 
	or die "unable to open ../config/floppy.conf\n";


	while (<$fd>)
	{
	   my $line = $_;
	   chomp($line);
	
	   if ( $line =~ /\[FILES\]/ ) { $STATE = 1; }
	   elsif ( $line =~ /\[GRUB\]/ ) { $STATE = 2; }
	   elsif ( ! ($line =~ /^\s*$/) )
	   {

	     if ( $STATE == 1 )
	     {
		if ( $line =~ /^(\/.*)\/(.*?)$/ )
		{
		  my $path = $1;
		  # ---------- CREATE THE TREE
		  my @hier;
		  my $bob = $path;
		  	while ( $bob =~ /^(\/.*)\/(.*?)$/ )
			{
			  push @hier,$1;
			  $bob = $1;
			}
		 
		 	while ( $bob = pop @hier )
			{
		  		mkdir("./tmp.floppy".$bob);
			}
		  	mkdir("./tmp.floppy".$path);
		  # ------------ DONE
		
		  my $filename = $line;
		  while ( $filename =~ s/.*\/// ) { }
		  print "gzipping $line --> ./tmp.floppy".$path."/$filename\n";
		   
		  die "copy failed!\n" if 
		   ( system("gzip -c $line > ./tmp.floppy/$path/$filename") != 0 );
		}
	     }
	     elsif ( $STATE == 2) 
	     {
	     	my $GRUB;
		if ( $GRUB_CLEAN == 0 )
		{
		  mkdir("./tmp.floppy/grub/");
		  open($GRUB,"> ./tmp.floppy/grub/grub.conf")
		   or die "Unable to open ./tmp.floppy/grub/grub.conf\n";
		  $GRUB_CLEAN = 1;
		}
		else
		{
		  open($GRUB,">> ./tmp.floppy/grub/grub.conf")
		   or die "Unable to open for appending ./tmp.floppy/grub/grub.conf\n";
		}
	    	print $GRUB $line . "\n";
		close($GRUB);
	     }
	     
	   }
	
	}

	close($fd);



  copy("./tmp.floppy/grub/grub.conf","./tmp.floppy/grub/menu.lst");
  
  copy("/boot/grub/stage1","./tmp.floppy/grub/") if ( -e "/boot/grub/stage1");
  copy("/boot/stage1","./tmp.floppy/grub/") if ( -e "/boot/stage1");
  copy("./stage1","./tmp.floppy/grub/") if ( -e "./stage1");

  copy("/boot/grub/stage2","./tmp.floppy/grub/") if ( -e "/boot/grub/stage2");
  copy("/boot/stage2","./tmp.floppy/grub/") if ( -e "/boot/stage2");
  copy("./stage2","./tmp.floppy/grub/") if ( -e "./stage2");


  die "unable to unmount ./tmp.floppy" if 
  ( system("/bin/umount ./tmp.floppy") != 0 );
  rmdir("./tmp.floppy");

  # ---------------------------------------

   my $GD;

	if ( -e "./grub" )
	{
		open($GD,"| ./grub") or 
		die "Unable to start piping into ./grub\n";
	}
	else
	{
		open($GD,"| /sbin/grub") or 
		die "Unable to start piping into /sbin/grub\n";
	}

	print $GD "device (fd0) $BUILD_DIR/floppy.img\n";
	print $GD "root (fd0)\n";
	print $GD "setup (fd0)\n";
	print $GD "quit\n";

	close($GD);

#---------------------------------------------------
print "****************************************************\n";
print "I'm pretty sure that everything worked.\n\n";
print "  $BUILD_DIR/floppy.img\n\n";
print "That's now a bootable floppy image made according\n";
print "to the floppy.conf file.\n\n";
print "If you want to write it to a real floppy:\n\n";
print "   dd if=$BUILD_DIR/floppy.img of=/dev/fd0\n";
print "\n";
print "good luck.\n"


