#!/usr/bin/perl -w

use strict;

#---------------------------------------

my $ARCHITECTURE 		= "i686";		# default


#---------------------------------------


  my $PARAM_COUNT = 0;
  my $HANDLED = 0;
  
    while (my $param = shift)
	{
		show_defaults() if ( $param eq "--show-defaults" );
		show() if ( $param eq "--show" );
		list() if ( $param eq "--list" );

		$HANDLED = 0;
		
		if ( $param =~ /--arch=(.*)/ )
		{
		  $ARCHITECTURE = $1;
		  $HANDLED = 1;
		}
		
 		die "unknown parameter: $param\n" if ( $HANDLED == 0 );
		
		$PARAM_COUNT += 1;
	};

	print_usage()  if ( $PARAM_COUNT == 0 );


	die "Unknown architecture!\n" if ( ! -e "config/make.$ARCHITECTURE");


		save_config();

       

exit(0);


# --------------------------------------


sub save_config
{
	open(FD,"> config/make.configure");
	  print FD "ARCH=$ARCHITECTURE\n";
	close(FD);
}


# --------------------------------------

sub list
{
   my @ARCH;

   #-- architectures ----------
   opendir(DIR,"config");
	while (my $entry = readdir(DIR) )
	{
	   next if ( ! ( $entry =~ /make[.]/ ) );
	   next if ( $entry eq "make.system" );
	   next if ( $entry eq "make.configure" );

		$entry =~ s/make[.]//;
		
		push @ARCH, $entry;
	}
   closedir(DIR);
   
   
   #--------------------------- 
   my $count = 0;

   print "Architectures:\n";
   while ( my $arch = pop @ARCH )
   {
     print "   $arch";
	 print "\n" if ( $count++ == 4 );
   }


   print "\n\n";


   exit(0);
}

sub show
{
   open (FD,"< ./config/make.configure" );

   while (<FD>)
   {
  	  my $line = $_;
	  chomp($line);
   	  	$ARCHITECTURE = $1 if ( $line =~ /^ARCH=(.*)/ );
   }

   close(FD);

   # -------------------------------


	print "Current configuration:\n";
	print "\n";
	print "     --arch=$ARCHITECTURE\n";
	print "\n";



   exit(0);
}


sub show_defaults
{
	print "Configuration defaults:\n";
	print "\n";
	print "     --arch=$ARCHITECTURE\n";
	print "\n";


	exit(0);
}


# --------------------------------------

sub print_usage
{
	print "./configure.pl [options] [command]\n";
	print "\n";
	print "   example:   ./configure.pl  --arch=$ARCHITECTURE\n";
	print "\n";
	print "\n";
	print "  Configuration options:\n";
	print "      --arch=[ARCH]\n";
	print "\n";
	print "  Commands:\n";
	print "      --list               display all options\n";
	print "      --show               show current settings\n";
	print "      --show-defaults      display defaults\n";

	print "\n";
	exit(-1);
}



