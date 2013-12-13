#!/usr/bin/perl -w

use strict;

my $ROOT_DIR	= "/spoon/system/source/";
my $CONFIG_DIR	= "/spoon/system/source/config/";
my $SCRIPT_DIR	= "/spoon/system/source/scripts/";
my $BUILDS_DIR	= "/spoon/system/source/builds/";

my $CONFIGURE_SCRIPT	= "/spoon/system/source/configure.pl";

my $COMMAND = "";

	print "WARNING: This is a slightly unresilient, non-user-friendly script. Are you sure you want to continue? (y/n) ";
	my $ans = <STDIN>;
	exit(0) if ( ! ($ans =~ /y/i) );

	save_command();
	print "restoration configuration: $COMMAND\n";

	my @ARCHS = per_arch();

   	while ( my $arch = pop @ARCHS )
	   {
	     print "Compiling releases for: $arch\n";
	     print "   setting $arch   ... ";
		 set_arch( $arch );
		 print " ok\n";
		 print "   compiling $arch ... ";
		 compile( $arch );
		 print " ok\n";
		 print "   floppy $arch    ... ";
		 floppy( $arch );
		 print " ok\n";
		 print "   iso $arch       ... ";
		 iso( $arch );
		 print " ok\n";
	   }

	print "restoring original configuration\n";
	restore_command();
	print "done\n";


exit(0);


# --------------------------------------

sub save_command
{
	chdir($ROOT_DIR);
	open(FD,"./configure.pl --command |") or die "configure script oops.\n";
	$COMMAND = <FD>;
	close(FD);
	return 0;
}

sub restore_command
{
	chdir($ROOT_DIR);
	system("./".$COMMAND);
	return 0;
}

sub per_arch
{
   my @ARCH;

   #-- architectures ----------
   opendir(DIR, $CONFIG_DIR );
	while (my $entry = readdir(DIR) )
	{
	   next if ( ! ( $entry =~ /make[.]/ ) );
	   next if ( $entry eq "make.system" );
	   next if ( $entry eq "make.configure" );
	   next if ( $entry eq "make.global" );

		$entry =~ s/make[.]//;
		
		push @ARCH, $entry;
	}
   closedir(DIR);
   
   
   #--------------------------- 
   return @ARCH;
}


sub set_arch
{
	my ($arch) = @_;

	chdir( $ROOT_DIR );

	if ( system("$CONFIGURE_SCRIPT --arch=$arch") != 0 )
	{
		die "unable to set architecture $arch\n";
	}

};

sub compile
{
	my ($arch) = @_;

	chdir( $ROOT_DIR );

	if ( system("make all > /dev/null 2> /dev/null && make install > /dev/null 2> /dev/null") != 0 )
	{
		die "unable to compile\n";
	}
}


sub floppy
{
	my ($arch) = @_;

	chdir( $SCRIPT_DIR );

	if ( system("sudo ./make_floppy.pl > /dev/null 2> /dev/null") != 0 )
	{
		die "unable to make floppy\n";
	}
	chdir( $BUILDS_DIR );
	system( "tar -czf ./floppy.$arch.tar.gz ./floppy.img > /dev/null 2> /dev/null" );
}

sub iso
{
	my ($arch) = @_;

	chdir( $SCRIPT_DIR );

	if ( system("./make_iso.pl > /dev/null 2> /dev/null") != 0 )
	{
		die "unable to make iso\n";
	}

	chdir( $BUILDS_DIR );
	system( "tar -czf ./cdrom.$arch.tar.gz ./cdrom.iso > /dev/null 2> /dev/null" );
}


